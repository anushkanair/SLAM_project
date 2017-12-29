DEPENDENCIES
Redis (C++, python)
hiredis (C++)
wiringPi (C++)
pyserial (python)

PWM_motor_controller.cpp
Description: Control two motors on pins 7 and 0 (wiringPi ids) using PWM. Reads 'motor_left_speed' and 'motor_right_speed' from
	Redis. Values are mapped from -3 to 3 integers to the PWM duty cycle.
Calibration: The PWM duties need to be calibrated to increment in roughly equal voltage outputs at the end of the RC filter
	and 3.3V to 5V converter. 2.5V corresponds to 0 speed, 5V corresponds to full speed one direction, 0V corresponds to 
	full speed the other direction.

arduino_listener.py
Description: Reads encoder values from Arduino board over serial. Posts left and right encoder values to keys 'motor_left_encoder'
	and 'motor_right_encoder' on Redis. Also reads the integrated robot position and orientation from the arduino. Writes to
	keys 'robot_xmm', 'robot_ymm' and 'robot_theta_deg'.
Input arguments: Takes the port address of the Arduino. Usually /dev/ttyACM0 or /dev/ttyACM1.
Setup: Connect Arduino mega running readEncoders.ino

move_robot.py
Description: Reads characters from keyboard and causes robot to move by posting speeds to redis for both motors.
	'w': move forward 0.5s, 's': move reverse 0.5s, 'a': turn left 0.5s, 'd': turn right 0.5s.

