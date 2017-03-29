#include <opencv2/opencv.hpp>
using namespace cv ;

class Marker{
	
	int x ;
	int y ;

	int H_MIN, S_MIN, V_MIN;
	int H_MAX, S_MAX, V_MAX;

	bool calibrationMode ;
	bool mouseIsDragging ;
	bool mouseMove ;
	bool rectangleSelected ;

	bool useMorphs ;
	bool trackObject ;
	bool objectVisible ;

	Point initialClickPoint, currentMousePoint ;
	Rect rectangleROI ;

	Mat threshold ;

	void recordHSV_values(Mat &frame, Mat &hsv_frame);
	void morphOps(Mat &thresh);
	void trackFilteredObject(Mat &threshold, Mat &cameraFeed);
	void drawObject(Mat &frame);
  
  public :

  	Marker();
	bool isCalibrated();
	bool getCalibrationMode();
  	void setCalibrationMode(bool flag);
  	void calibrate(int event, int x, int y, Mat* frame, Mat &hsv_frame);
  	void performTrackingOperations(Mat &HSV, Mat &cameraFeed);
	Point getCoordinates();
	bool isVisible();
};