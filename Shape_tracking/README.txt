DESCRIPTION
shapeTracking.cpp: Detect a triangle based on Canny edge algorithm and polygon approximation of visible contours. Posts
	coordinates of the triangle to Redis. Note that the inner contour of the triangle is identified,

shapeDetection.cpp: Identifies basic polygonal shapes in an input image. 

save_img.cpp: Takes in a filename and saves an image from the attached webcam to a file.

DEPENDENCIES
OpenCV 3+
Redis

SETUP
Camera calibration:
	Write distortion coefficients to distort_coeffs, and the camera matrix to camera_mat. We also need the inverse of the
	camera matrix in camera_mat_inv. The current camera calibration values were computed using a chessBoard.
	See opencv/samples/cpp/tutorial_code/calib3d/camera_calibration/.

Initialization:
	Run redis-server.

EXPECTED BEHAVIOR
If multiple triangles are in the scene, the algorithm outputs the largest triangle.
Vertex 1 is to the left and top of the triangle and vertex 2 is the right-most.
Redis keys for vertices: 'triangle_vertex1', 'triangle_vertex2', 'triangle_vertex3'

