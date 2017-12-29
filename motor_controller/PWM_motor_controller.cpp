#include <wiringPi.h>
#include <softPwm.h>
#include <hiredis/hiredis.h>

#include <iostream>
#include <string>

using namespace std;

/*constants*/
const string motor_left_speed_key = "motor_left_speed";
const string motor_right_speed_key = "motor_right_speed";
const uint motor_left_pin = 7;
const uint motor_right_pin = 0;

/* global variables */ 

int motor_left_speed = 0;
int motor_right_speed = 0;
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

bool redis_read_int (int& ret_int, const string& key) {
	reply = (redisReply *)redisCommand(redis, "GET %s", key.c_str());
	if (NULL == reply || REDIS_REPLY_ERROR == reply->type) {
		return false;
	}
	ret_int = stoi(reply->str);
}

int get_PWM_left (int speed) {
	switch (speed) {
		case -3 : return 0;
		case -2 : return 15;
		case -1 : return 25;
		case 0 : return 34;
		case 1 : return 39;
		case 2 : return 46;
		case 3 : return 50;
		default : return 34;
	}
}

int get_PWM_right (int speed) {
	switch (speed) {
		case -3 : return 0;
		case -2 : return 15;
		case -1 : return 25;
		case 0 : return 34;
		case 1 : return 39;
		case 2 : return 46;
		case 3 : return 50;
		default : return 34;
	}
}

int main (void) {
	// set up wiring pi
	wiringPiSetup ();
	softPwmCreate (motor_left_pin, get_PWM_left (0), 50);
	softPwmCreate (motor_right_pin, get_PWM_right (0), 50);

	// initiate redis
	redis_init();

	

	while (true) {
		delay (10);
		//every 10 milliseconds - read from redis and then write the pwm values
		// get motor speeds
		bool success = redis_read_int(motor_left_speed, motor_left_speed_key);
		success &= redis_read_int(motor_right_speed, motor_right_speed_key);
		if (!success) {
			// fall back to 0 speed
			motor_left_speed = 0;
			motor_right_speed = 0;
		}
		//get PWM
		softPwmWrite (motor_left_pin, get_PWM_left (motor_left_speed));
		softPwmWrite (motor_right_pin, get_PWM_right (motor_right_speed));
	}
}
