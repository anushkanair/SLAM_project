#include "opencv2/opencv.hpp"
#include "string"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    	VideoCapture cap(0); // open the default camera
    	if(!cap.isOpened()) // check if we succeeded
    	{ return -1; }
	// Get the frame
	Mat save_img; cap >> save_img;

	if(save_img.empty())
	{
  		std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
	}
	// Save the frame into a file
	imwrite((string(argv[1])+".jpg").c_str(), save_img); // A JPG FILE IS BEING SAVED
}

