/* Read encoder and write to serial
 */

#include <Encoder.h>

// encoder construct
Encoder motorLeft(2, 3); // on interrupt pins
Encoder motorRight(18, 19); // on interrupt pins

// serial setup
void setup() {
  Serial.begin(9600);
//  Serial.println("Encoder Test:");
}

// encoder variables
long positionLeft  = motorLeft.read();
long positionRight = motorRight.read();
long newLeft, newRight;

// serial variables
char message[60];
int counter = 0;

// robot constants
const double enc_counts_to_rads = 2.0*M_PI*1.0/131.0*1.0/64.0;
const double secs_to_mills = 1e3;
const double rads_to_degs = 180.0/M_PI;
const double wheel_size_mm = 45.0;
const double wheel_wheel_distance = 298.45;

// robot motion variables
double robot_x = 0;
double robot_y = 0;
long robot_x_mm, robot_y_mm, robot_theta_deg;
double robot_theta = M_PI/2.0; // radians
double robot_local_y_speed, robot_theta_speed;
double wheel_rot_speed_left, wheel_rot_speed_right;
double wheel_trans_speed_left, wheel_trans_speed_right;

// timing variables
double dt_secs;
long long last_time = millis();
long long curr_time;

void loop() {
  delay(10); // 0.01 secs
  newLeft = motorLeft.read();
  newRight = motorRight.read();
  ++counter;

  // compute instantaneous wheel speeds
  curr_time = millis();
  dt_secs = (curr_time - last_time)/secs_to_mills;
  last_time = curr_time;
  wheel_rot_speed_left = (newLeft - positionLeft)*enc_counts_to_rads/dt_secs;
  wheel_rot_speed_right = -(newRight - positionRight)*enc_counts_to_rads/dt_secs;
  // TODO: moving average filter wheel speeds
  // compute robot translation, rotation speed
  wheel_trans_speed_left = wheel_rot_speed_left*wheel_size_mm;
  wheel_trans_speed_right = wheel_rot_speed_right*wheel_size_mm;
  robot_local_y_speed = (wheel_trans_speed_left + wheel_trans_speed_right)/2.0;
  robot_theta_speed = (wheel_trans_speed_right - wheel_trans_speed_left)/wheel_wheel_distance;
  
  // integrate to obtain robot position
  robot_x += cos(robot_theta)*robot_local_y_speed*dt_secs;
  robot_y += sin(robot_theta)*robot_local_y_speed*dt_secs;
  robot_theta += robot_theta_speed*dt_secs;
  robot_x_mm = robot_x; robot_y_mm = robot_y;
  robot_theta_deg = robot_theta*rads_to_degs;

  // serial write
  positionLeft = newLeft;
  positionRight = newRight;

  if (counter > 10) { // once every 0.1 secs
    // communicate over serial
    sprintf(message, "E%+011ld%+011ld%+011ld%+011ld%+011ld", positionLeft, positionRight, robot_x_mm, robot_y_mm, robot_theta_deg);
    Serial.print(message);
    Serial.println();
    counter = 0;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
//  if (Serial.available()) {
//    Serial.read();
//    Serial.println("Reset both encoder counts to zero");
//    motorLeft.write(0);
//    motorRight.write(0);
//  }
}

