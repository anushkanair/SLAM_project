#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat src, src_gray, blurred, edge_img, src_resized;
double alpha = 1.0;
double beta = 0;

//load the image and resize it to a smaller factor so that
//the shapes can be approximated better
int main (int argc, char** argv) {
    src= imread(argv[1]);
    resize(src, src_resized, Size(600, 600));
    Mat new_image = Mat::zeros(src_resized.size(), src_resized.type());
    double ratio = 1.0;

    
    //converts to grayscale, blurs the image, and sets a threshold
    // Mat channels[3];
    // for( int y = 0; y < src_resized.rows; y++ ) {
    //     for( int x = 0; x < src_resized.cols; x++ ) {
    //        new_image.at<Vec3b>(y,x)[2] =
    //        saturate_cast<uchar>( alpha*(src_resized.at<Vec3b>(y,x)[2] ) + beta );
    //     }
    // }
    //split(new_image, channels);
    //cvtColor(src_resized, src_gray, COLOR_BGR2GRAY);
    blur(src_resized, blurred, Size(5, 5));
    //if the color is above a threshold it turns black, otherwise it is white
    //threshold(blurred, edge_img, 150, 255, THRESH_BINARY_INV);

    //show the threshold image

    //Use Canny edge detection
    Canny(blurred, edge_img, 40, (40)*3, 3);
    blur(edge_img, edge_img, Size(5, 5));

    namedWindow( "Canny", WINDOW_NORMAL );
    imshow("Canny", edge_img);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //Find contours of the image
    findContours(edge_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<Moments> mu(contours.size() );
    vector<vector<Point> > approxs(contours.size() );
    Mat drawing = Mat::zeros(edge_img.size(), CV_8UC3 );

    //cout << contours.size() << endl;

    int candidate = 0;

    for(int i = 0; i < contours.size(); i++)
    {
      //find the moments, and the X and Y components
      mu[i] = moments( contours[i], false );
      double X = int(mu[i].m10/mu[i].m00);
      double Y = int(mu[i].m01/mu[i].m00);
      string Shape;
      double peri = arcLength(contours[i], true);
      //using the contours and approxs to approximate the polygons curves
      approxPolyDP(contours[i], approxs[i], 0.04 * peri , true);
      vector<Point> approx = approxs[i];
      //using the size of the approx vector, estimate what shape it is
      if (approx.size()== 3)
          {
            Shape = "triangle";
            RotatedRect rectangle= minAreaRect(approx);
            double ar = rectangle.size.width / rectangle.size.height;
            if (ar <= 0.2 || ar >= 3) {
              continue;
            }
            cout << ar << endl;
            if (approxs[candidate].size() != 3 || contourArea(contours[i]) > contourArea(contours[candidate])) {
               candidate = i;
            }
          }
      else if (approx.size() == 4)
          {
            Rect2d rectangle= boundingRect(approx);
            double ar = rectangle.width / rectangle.height;
            if (ar >= 0.95 && ar <= 1.05)
            {Shape = "Square";}
            else
            {Shape = "rectangle";}
          }
      else if (approx.size() == 5)
          {Shape = "pentagon";}
      else
        {Shape = "circle" + to_string(approx.size());}

      cout << Shape <<endl;


      drawContours(drawing, approxs, i, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point() );
      putText(drawing, Shape, Point(X, Y), FONT_HERSHEY_SIMPLEX,	0.5, Scalar(255, 255, 255), 2);


    }

      //draw the contours of the image
      drawContours(drawing, contours, candidate, Scalar(0, 255, 0), 2, 8, hierarchy, 0, Point() );

      //display the images
      namedWindow( "Contours", WINDOW_NORMAL );
      imshow( "Contours", drawing );
      waitKey(0);
    }
