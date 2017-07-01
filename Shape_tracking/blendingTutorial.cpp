#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

/*
Adding and Blending Images Tutorial
December 20, 2016
OpenCv : http://docs.opencv.org/trunk/d5/dc4/tutorial_adding_images.html
*/

using namespace cv;

int main( void )
{
   //Initialize Variables
   double alpha = 0.5; double beta; double input;
   Mat src1, src2, dst;

   //Get Alpha Value
   std::cout<<"Enter an Alpha Value between 0 and 1: ";
   std::cin>>input;
   if( alpha >= 0 && alpha <= 1 )
     { alpha = input; }
   else
   {std::cout << "Invalid Alpha Value. Please try again."; }

   //download the two images
   src1 = imread("../data/LinuxLogo.jpg");
   src2 = imread("../data/WindowsLogo.jpg");
   if( src1.empty() ) { std::cout<< "Error loading src1"<<std::endl; return -1; }
   if( src2.empty() ) { std::cout<< "Error loading src2"<<std::endl; return -1; }

   //Blend the images by adding the weighted vectors
   beta = ( 1.0 - alpha );
   addWeighted( src1, alpha, src2, beta, 0.0, dst);

   //Show the image and wait
   imshow( "Linear Blend", dst );
   waitKey(0);
   return 0;
}
