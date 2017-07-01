#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;

//two thresholds
int thresh = 100;
int max_thresh = 255;

//random number generator
RNG rng(12345);

void thresh_callback(int, void* );

int main( int, char** argv )
{
  src = imread( argv[1], IMREAD_COLOR );
  //convert color
  cvtColor( src, src_gray, COLOR_BGR2GRAY );
  //blur the image
  blur( src_gray, src_gray, Size(3,3) );

  const char* source_window = "Source";

  //
  namedWindow( source_window, WINDOW_AUTOSIZE );
  imshow( source_window, src );


  createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );
  waitKey(0);
  return(0);
}
void thresh_callback(int, void* )
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  //canny edge detection
  Canny( src_gray, canny_output, thresh, thresh*2, 3);
  namedWindow( "Canny Output", WINDOW_AUTOSIZE );
  imshow( "Canny Output", canny_output );

  findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  //vector<Moments> mu(contours.size() );
  //for every contour find the moments
  // for( size_t i = 0; i < contours.size(); i++ )
  //    { mu[i] = moments( contours[i], false ); }
  // vector<Point2f> mc( contours.size() );
  // for( size_t i = 0; i < contours.size(); i++ )
  // //find out what .m10 and m.oo meamn
  //    { mc[i] = Point2f( static_cast<float>(mu[i].m10/mu[i].m00) , static_cast<float>(mu[i].m01/mu[i].m00) ); }
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
       //draw a circle
       //circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }


  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );

  printf("\t Info: Area and Contour Length \n");
  for( size_t i = 0; i< contours.size(); i++ )
     {
       //printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", (int)i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 )
     }
}
