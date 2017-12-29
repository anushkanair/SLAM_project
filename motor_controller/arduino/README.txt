DEPENDENCY
Encoder library for arduino: https://github.com/PaulStoffregen/Encoder
	Available for install on latest Arduino IDE.

DESCRIPTION
readEncoder.ino: Read 2 encoders connected to the SLAM robot on hardware interrup pins of the Mega. Compute instantaneous wheel
	speed, robot speed and integrate to get robot x, y and orientation with respect to initial pose.
	Sends a message over serial of the format 'E <encoder left 11 digits signed> <encoder right 11 digits signed> 
	<robot x in mm 11 digits signed> <robot y in mm 11 digits signed> <robot theta in deg 11 digits signed>'

SETUP
Constant variables
	Encoder initialization requires pin numbers on which channel A and B of the two encoders are connected.

	robot_wheel_radius: radius of wheel including tire in mm.

	robot_wheel_wheel_distance: distance between the two wheel centers in mm.

Timing
	By default, the encoders are read at a frequency of 100Hz to compute speeds and robot pose updates.
	The messages are sent over serial at 10Hz.

