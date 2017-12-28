DEPENDENCIES
- Numpy
- Scipy
- Redis (both python)
- transitions (library for state machine)

SETUP
- Calibrate constant variables
	ALPHA_THRESHOLD: Tolerance for angular misalignment with the triangle

	MOVE_THRESHOLD: Tolerance for distance from triangle at which robot should be

	ROBOT_TRI_DISTANCE: Desired distance from triangle for robot to be at

	TF_CAMERA_TO_BASE_ROT: Rotation matrix denoting camera coordinates in robot base coordinates
	(Note: Robot x = looking to right, Robot y = looking forward)

	TF_CAMERA_TO_BASE_TRANS: Position of camera sensor in robot base coordinates
	(Note: Robot base origin is in between the two wheel centers)

	TRI_L12, TRI_L23, TRI_13: length of triangle side 12, 23 and 13 respectively
	(Note: Vertex 1 is assumed to be the top-most and vertex 2 is assumed to be the right-most as seen by the camera)
	(Note: The actual lengths need to be scaled down a bit as the triangle detected is slightly inside the actual triangle)

	ROBOT_TRANS_MM_PER_SECOND: How much the robot moves forward in one second when both wheels are at speed 1.
	ROBOT_ROTATE_DEG_PER_SECOND: How much the robot rotates in one second when one wheel is run forward and the other 
	backward at speed 1.
	(Note: These must be recalibrated if the motor battery voltage drops. They will not required to be changed if the robot is 
	run with speed control.)

- Initialization
	1. Start redis, motor pwm controller, encoder driver and triangle sensor codes
	2. Position robot to be able to see the triangle in its frame
	3. Ensure no cables can get entangled or taut as the robot moves to its goal

RUNNING PROGRAM
$ python moveToTriangle.py

EXPECTED BEHAVIOR
Robot should first turn in the direction it needs to move, then move in a straight line. After reaching destination, it will 
attempt to rotate to first find the triangle and then center it in its frame. This behavior is repeated in a loop.
Ctrl+C kills the program and stops the robot.
