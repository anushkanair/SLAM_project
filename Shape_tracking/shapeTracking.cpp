#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened()) // check if we succeeded
    { return -1; }
    Mat edges;

    namedWindow( "Unprocessed", WINDOW_NORMAL );

    namedWindow("edges",1);
    namedWindow("canny_out",1);
    for(;;) //for loop with no condition
    {
        Mat frame, blurred, edge_img;

        cap >> frame; // get a new frame from camera
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5)
        //add triangle code here

        imshow("Unprocessed", frame);

        blur(frame, blurred, Size(5, 5));
        Canny(blurred, edge_img, 40, (40)*3, 3);
        blur(edge_img, edge_img, Size(5, 5));
        imshow("canny_out", edge_img);
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


          //drawContours(drawing, contours, i, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point() );

        }
        if (Shape.empty()) {
          // drop frame if triangle not found
          continue;
        }
          //draw the contours of the image
          drawContours(drawing, approxs, candidate, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point() );

          imshow("edges", drawing);
          if(waitKey(30) >= 0) break; //wait 30 milliseconds if it gets a key stroke
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
