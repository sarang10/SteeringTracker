#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Marker.h"
#include "Constants.h"
#include <cstdio>

using namespace std;
using namespace cv ;

void Marker::recordHSV_values(Mat &frame, Mat &hsv_frame){
	
	if(mouseMove==false && rectangleSelected){
		if(rectangleROI.width<1 || rectangleROI.height<1){
			fprintf(stderr, "Please draw a rectangle,not a line.\n") ;
			return ;
		}
		H_MIN = S_MIN = V_MIN = 256 ;
		H_MAX = S_MAX = V_MAX = 0 ;

		int H,S,V ;
		for(int i=rectangleROI.x ; i<rectangleROI.x+rectangleROI.width ; i++)
			for(int j=rectangleROI.y ; j<rectangleROI.y+rectangleROI.height ; j++){

				H = (int)hsv_frame.at<Vec3b>(j,i)[0] ;
				S = (int)hsv_frame.at<Vec3b>(j,i)[1] ;
				V = (int)hsv_frame.at<Vec3b>(j,i)[2] ;

				H_MIN = min(H_MIN , H) ;
				H_MAX = max(H_MAX , H) ;

				S_MIN = min(S_MIN , S) ;
				S_MAX = max(S_MAX , S) ;

				V_MIN = min(V_MIN , V) ;
				V_MAX = max(V_MAX , V) ;
			}
		rectangleSelected = false ;
		calibrationMode = false ; 
		cout<<"HSV_recorded...\n" ;	
	}
	if(mouseMove){
		cout<<"Drawing rectangle\n" ;
		rectangle(frame, initialClickPoint, Point(currentMousePoint.x,currentMousePoint.y),Scalar(0,255,0),1,8,0) ;	
	}
	
}

void Marker::morphOps(Mat &thresh){
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3,3)) ;
	Mat dilateElement = getStructuringElement(MORPH_RECT,Size(8,8)) ;

	erode(thresh, thresh, erodeElement) ;
	erode(thresh, thresh, erodeElement) ;

	dilate(thresh, thresh, dilateElement) ;
	dilate(thresh, thresh, dilateElement) ;
}

void Marker::trackFilteredObject(Mat &threshold, Mat &cameraFeed){

	vector< vector<Point> > contours ;
	vector<Vec4i> heirarchy;

	findContours(threshold, contours, heirarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	double refArea = 0 ;
	bool objectFound = false ;

	if(heirarchy.size() > 0){
		int numObjects = heirarchy.size() ;
		if(numObjects<MAX_NUM_OBJECTS){
			
			for (int index = 0; index >= 0; index = heirarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]) ;
				double area = moment.m00 ;

				if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area ;
					y = moment.m01/area ;
					objectFound = true ;
					refArea = area ;
				}
				else 
					objectFound = false ;
			}
			if(objectFound ==true)
				drawObject(cameraFeed);}
			else{ 
				  // putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
			}
	}
	objectVisible = objectFound ;
}

void Marker::drawObject(Mat &frame){
	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
	if(y-25>0)
		line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	else
		line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
	if(y+25<FRAME_HEIGHT)
		line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	else
	    line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
	if(x-25>0)
		line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	else
	    line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
	if(x+25<FRAME_WIDTH)
		line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
	else 
		line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	char str[30] ;
	sprintf(str,"( %d,%d )",x,y) ;
	putText(frame, str, Point(x,y+30), 1, 1, Scalar(0,255,0), 2);
}

Marker::Marker(){

	H_MIN = S_MIN = V_MIN = 0 ;
	H_MAX = S_MAX = V_MAX = 256 ;
	calibrationMode = true ;
	mouseIsDragging = mouseMove = rectangleSelected = false ;
	useMorphs = trackObject = true ;
}

bool Marker::isCalibrated(){
	return calibrationMode==false ;
}
bool Marker::getCalibrationMode(){
	return calibrationMode ;
}

void Marker::setCalibrationMode(bool flag){
	calibrationMode = flag ;
}

void Marker::calibrate(int event, int x, int y, Mat* frame, Mat &hsv_frame){
	
	if(!calibrationMode)
		return ;
	if(event==CV_EVENT_LBUTTONDOWN && mouseIsDragging==false){
		initialClickPoint = Point(x,y) ;
		mouseIsDragging = true;
	}
	if(event==CV_EVENT_MOUSEMOVE && mouseIsDragging){
		currentMousePoint = Point(x,y) ;
		mouseMove = true ;
		// recordHSV_values(frame, hsv_frame);
	}
	if(event==CV_EVENT_LBUTTONUP && mouseIsDragging){
		rectangleROI = Rect(initialClickPoint,currentMousePoint) ;
		mouseMove = mouseIsDragging = false ;
		rectangleSelected = true ;
		// recordHSV_values(frame, hsv_frame);	
	}
}

void Marker::performTrackingOperations(Mat &HSV, Mat &cameraFeed){
	inRange(HSV, Scalar(H_MIN,S_MIN,V_MIN), Scalar(H_MAX,S_MAX,V_MAX), threshold) ;

	recordHSV_values(cameraFeed, HSV) ;
	if(calibrationMode)
		return ;
	if(useMorphs)
		morphOps(threshold) ;
	if(trackObject)
		trackFilteredObject(threshold, cameraFeed) ;
}

Point Marker::getCoordinates(){
	return Point(x, y) ;
}

bool Marker::isVisible(){
	return objectVisible == true; 	
}
