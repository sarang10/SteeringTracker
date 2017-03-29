#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Marker.h"
#include "Constants.h"

using namespace cv ;
using namespace std;

const std::string windowName = "SteeringTracker" ;
Mat cameraFeed, HSV ;
Marker marker1, marker2 ,marker3, marker4;

double getSteeringTiltAngle(Point p1, Point p2){
	double slope = double(p2.y-p1.y)/(p2.x-p1.x) ;
	return atan(slope) * 180/PI ;
}

void clickAndDrag_Rectangle(int event, int x, int y, int flags, void* param){
	if(! marker1.isCalibrated()){
		marker1.calibrate(event, x, y, (Mat*)param, HSV) ;
	}
	else if(! marker2.isCalibrated()){
		marker2.calibrate(event, x, y, (Mat*)param, HSV) ;
	}
	else if(! marker3.isCalibrated()){
		marker3.calibrate(event, x, y, (Mat*)param, HSV) ;
	}
	else if(! marker4.isCalibrated()){
		marker4.calibrate(event, x, y, (Mat*)param, HSV) ;
	}
}
bool isTurningRight = false ;
bool isTurningLeft = false ;
bool areBrakesApplied = true ;
bool isAccelarating = false ;

void moveTheCar(double tiltAngle){
	
	/*Handling Up Arrow Key*/
	if(marker3.isVisible() && isAccelarating){
		cout<<"AcceleratorMarker is isVisible = true;Leaving accelerator\n";
		system("xdotool keyup Up");
		isAccelarating = false ;
	}
	else if( !marker3.isVisible() && !isAccelarating){
		cout<<"AcceleratorMarker is not visible ; PressingAccelerator\n";
		system("xdotool keydown Up");
		isAccelarating = true ; 
	}
	
	/*Handling Down Arrow Key*/
	if(marker4.isVisible() && areBrakesApplied){
		cout<<"BrakeMarker is isVisible = true;Leaving brakes\n";
		system("xdotool keyup Down");
		areBrakesApplied = false ;
	}
	else if( !marker4.isVisible() && !areBrakesApplied){
		cout<<"BrakeMarker is not visible ; Applying Brakes\n";
		system("xdotool keydown Down");
		areBrakesApplied = true ; 
	}	

	/*Handling right turn event*/
	if(tiltAngle<(-MIN_TILT_ANGLE)){	
		putText(cameraFeed, "Turn Right", Point(5,50), 1, 1, Scalar(255,0,0), 2);
		
		if(!isTurningRight){		
			//Either it is moving straight or turning left, so stop it from turning left and start turning right.
			system("xdotool keyup Left keydown Right");		
			isTurningLeft = false ;
			isTurningRight = true ;
		}
	}
	else if(tiltAngle>MIN_TILT_ANGLE){
		putText(cameraFeed, "Turn Left", Point(5,50), 1, 1, Scalar(255,0,0), 2);
		if(!isTurningLeft){		
			//Either it is moving straight or turning right, so stop it from turning right and start turning left. 
			system("xdotool keyup Right keydown Left");		
			isTurningLeft = true ;
			isTurningRight = false ;
		}
	}
	else{
		putText(cameraFeed, "Straight", Point(5,50), 1, 1, Scalar(255,0,0), 2);
		if(isTurningLeft || isTurningRight){
			system("xdotool keyup Left keyup Right");
			isTurningLeft = false ;
			isTurningRight = false ;		
		}
	}
}

void mirrorCameraFeed(){		//To calibrate markers easily
	int cols = FRAME_WIDTH ;
	int rows = FRAME_HEIGHT ;
	Vec3b temp ;
	for(int i=0 ; i<FRAME_HEIGHT ; i++)
		for(int j=0 ; j<=FRAME_WIDTH/2 ; j++){
			temp = cameraFeed.at<Vec3b>(i,j) ;
			cameraFeed.at<Vec3b>(i,j) = cameraFeed.at<Vec3b>(i,FRAME_WIDTH-j) ;
			cameraFeed.at<Vec3b>(i,FRAME_WIDTH-j) = temp ;
		}			
}

int main(){
	
	VideoCapture capture(0) ;
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	namedWindow(windowName);
	setMouseCallback(windowName, clickAndDrag_Rectangle, &cameraFeed);

	double tiltAngle ;
	while(1){
		
		capture.read(cameraFeed);
		mirrorCameraFeed() ;
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);

		//Calibration will be performed in clickAndDrag function
		marker1.performTrackingOperations(HSV, cameraFeed);
		marker2.performTrackingOperations(HSV, cameraFeed);		
		marker3.performTrackingOperations(HSV, cameraFeed);
		marker4.performTrackingOperations(HSV, cameraFeed);
                
		if(marker1.isCalibrated() && marker2.isCalibrated() && marker3.isCalibrated() && marker4.isCalibrated()){
			
			Point p1 = marker1.getCoordinates() ;
			Point p2 = marker2.getCoordinates() ;

			line(cameraFeed,p1,p2,Scalar(255,0,0),2);
			
			tiltAngle = getSteeringTiltAngle(p1, p2) ;
			moveTheCar(tiltAngle) ;
		}
			
		imshow(windowName, cameraFeed);	

		switch(waitKey(30)){
			case 27 : 
				return 0 ;
			case 'c' : 
					marker1.setCalibrationMode(true);
					marker2.setCalibrationMode(true);
					marker3.setCalibrationMode(true);
					marker4.setCalibrationMode(true);
					cout<<"Please calibrate the markers." ; 
				break; 	
		}

	}


}
