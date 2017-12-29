#include "opencv2/opencv.hpp"
#include "unistd.h"
#include "hiredis/hiredis.h"
#include <string>
using namespace cv;
using namespace std;

/*constants*/
const string triangle_vertex1_key = "triangle_vertex1";
const string triangle_vertex2_key = "triangle_vertex2";
const string triangle_vertex3_key = "triangle_vertex3";

/*global variables*/
redisContext *redis;
redisReply *reply;

void redis_init () {
	timeval timeout = {1, 500000}; // {seconds, microseconds}
	redis = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
	if (redis->err) {
		cerr << "Cannot connect to redis server!" << endl;
		exit(1);
	}	
}

bool redis_write_point (Mat point, const string& key) {
	string value = "";
	value += to_string(point.at<double>(0,0));
	value += " " + to_string(point.at<double>(1,0));
	value += " " + to_string(point.at<double>(2,0));
	reply = (redisReply *)redisCommand(redis, "SET %s %s", key.c_str(), value.c_str());
	if (REDIS_REPLY_ERROR == reply->type) {
		return false;
	}
	return true;
}

int main(int, char**)
{
	// initialize redis
	redis_init();

	VideoCapture cap(0); // open the default camera
	if(!cap.isOpened()) // check if we succeeded
	{ return -1; }

	// load camera calibration
	Mat camera_mat (3, 3, CV_64F);
	camera_mat.setTo(0.0);
	camera_mat.at<double>(0,0) = 6.0639429953043373e+02;
	camera_mat.at<double>(1,1) = 6.0639429953043373e+02;
	camera_mat.at<double>(2,2) = 1.0;
	camera_mat.at<double>(0,2) = 320.0;
	camera_mat.at<double>(1,2) = 240.0;

	Mat camera_mat_inv (3, 3, CV_64F);
	camera_mat_inv.setTo(0.0);
	camera_mat_inv.at<double>(0,0) = 1.0/6.0639429953043373e+02;
	camera_mat_inv.at<double>(1,1) = 1.0/6.0639429953043373e+02;
	camera_mat_inv.at<double>(2,2) = 1.0;
	camera_mat_inv.at<double>(0,2) = -1.0/6.0639429953043373e+02*320;
	camera_mat_inv.at<double>(1,2) = -1.0/6.0639429953043373e+02*240;

	Mat distort_coeffs (5, 1, CV_64F);
	distort_coeffs.at<double>(0,0) = 0.0;//7.0504001243685208e-02;
	distort_coeffs.at<double>(1,0) = 0.0;//1.0518563277654060e-01;
	distort_coeffs.at<double>(2,0) = 0;
	distort_coeffs.at<double>(3,0) = 0;
	distort_coeffs.at<double>(4,0) = 0.0;//-5.3173028361269215e-01;

    	Mat raw_frame, frame, blurred, edge_img;

    	//namedWindow( "Unprocessed", WINDOW_NORMAL );
    	//namedWindow("canny_out",1); 
    	namedWindow("edges",1);
    for(;;) //for loop with no condition
    {
        cap >> raw_frame; // get a new frame from camera

	// apply calibration
	undistort(raw_frame, frame, camera_mat, distort_coeffs);

        //add triangle code here
        //imshow("Unprocessed", frame);
        blur(frame, blurred, Size(5, 5));
        Canny(blurred, edge_img, 40, (40)*3, 3);
        blur(edge_img, edge_img, Size(5, 5));
        //imshow("canny_out", edge_img);
	//waitKey(1);
        //if the color is above a threshold it turns black, otherwise it is white
        //threshold(blurred, edge_img, 150, 255, THRESH_BINARY_INV);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        //Find contours of the image
        findContours(edge_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        vector<Moments> mu(contours.size() );
        vector<vector<Point> > approxs(contours.size() );
        Mat drawing = Mat::zeros(edge_img.size(), CV_8UC3 );

        //cout << contours.size() << endl;

        int candidate = 0;
        string Shape = "";

        for(int i = 0; i < contours.size(); i++)
        {
          //find the moments, and the X and Y components
          mu[i] = moments( contours[i], false );
          double X = int(mu[i].m10/mu[i].m00);
          double Y = int(mu[i].m01/mu[i].m00);
          double peri = arcLength(contours[i], true);
          //using the contours and approxs to approximate the polygons curves
          approxPolyDP(contours[i], approxs[i], 0.02 * peri ,true);
          vector<Point> approx = approxs[i];
          //using the size of the approx vector, estimate what shape it is
          if (approx.size()== 3)
              {
                RotatedRect rectangle= minAreaRect(approx);
                double ar = rectangle.size.width / rectangle.size.height;
                if (ar <= 0.4 || ar >= 2.5) {
                  continue;
                }
                //cout << ar << endl;
                if (approxs[candidate].size() != 3 || contourArea(contours[i]) > contourArea(contours[candidate])) {
                   Shape = "triangle";
                   candidate = i;
                }
              }
          // else if (approx.size() == 4)
          //     {
          //       Rect2d rectangle= boundingRect(approx);
          //       double ar = rectangle.width / rectangle.height;
          //       if (ar >= 0.95 && ar <= 1.05)
          //       {Shape = "Square";}
          //       else
          //       {Shape = "rectangle";}
          //     }
          // else if (approx.size() == 5)
          //     {Shape = "pentagon";}
          // else
          //   {Shape = "circle" + to_string(approx.size());}

          //cout << Shape <<endl;


          drawContours(drawing, contours, i, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point() );

        }
        if (Shape.empty()) {
          // drop frame if triangle not found
          continue;
        } else {
		// check for inner triangle
		if (hierarchy[candidate][2] != -1) {
			candidate = hierarchy[candidate][2];
			double peri = arcLength(contours[candidate], true);
          		//using the contours and approxs to approximate the polygons curves
          		approxPolyDP(contours[candidate], approxs[candidate], 0.02 * peri ,true);
		}

		uint v1 = 0;
		uint v2 = 1;
		uint v3 = 2;
		// determine vertex order
		if (approxs[candidate][0].x > approxs[candidate][1].x && approxs[candidate][0].x > approxs[candidate][2].x) {
			v2 = 0;
			if (approxs[candidate][1].y > approxs[candidate][2].y) {
				v1 = 2;
				v3 = 1;
			} else {
				v3 = 2;
				v1 = 1;
			}
		} else if (approxs[candidate][1].x > approxs[candidate][2].x && approxs[candidate][1].x > approxs[candidate][0].x) {
			v2 = 1;
			if (approxs[candidate][0].y > approxs[candidate][2].y) {
				v1 = 2;
				v3 = 0;
			} else {
				v3 = 2;
				v1 = 0;
			}
		} else {
			v2 = 2;
			if (approxs[candidate][0].y > approxs[candidate][1].y) {
				v1 = 1;
				v3 = 0;
			} else {
				v3 = 1;
				v1 = 0;
			}
		}

		// print the points
		Mat camera_point(3,1,CV_64F);
		Mat pixel_point(3,1,CV_64F);
		pixel_point.at<double>(0,0) = approxs[candidate][v1].x;
		pixel_point.at<double>(1,0) = approxs[candidate][v1].y;
		pixel_point.at<double>(2,0) = 1;
		camera_point = camera_mat_inv * pixel_point;
		//cout << "Point 1: " << camera_point.at<double>(0,0) << "," << camera_point.at<double>(1,0) << endl;
		redis_write_point (camera_point, triangle_vertex1_key);
		pixel_point.at<double>(0,0) = approxs[candidate][v2].x;
		pixel_point.at<double>(1,0) = approxs[candidate][v2].y;
		pixel_point.at<double>(2,0) = 1;
		camera_point = camera_mat_inv * pixel_point;
		//cout << "Point 2: " << camera_point.at<double>(0,0) << "," << camera_point.at<double>(1,0) << endl;
		redis_write_point (camera_point, triangle_vertex2_key);
		pixel_point.at<double>(0,0) = approxs[candidate][v3].x;
		pixel_point.at<double>(1,0) = approxs[candidate][v3].y;
		pixel_point.at<double>(2,0) = 1;
		camera_point = camera_mat_inv * pixel_point;
		//cout << "Point 2: " << camera_point.at<double>(0,0) << "," << camera_point.at<double>(1,0) << endl;
		redis_write_point (camera_point, triangle_vertex3_key);
		
	}
          //draw the contours of the image
          drawContours(drawing, approxs, candidate, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point() );

          imshow("edges", drawing);
	waitKey(1);
          //if(waitKey(30) >= 0) break; //wait 30 milliseconds if it gets a key stroke
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
