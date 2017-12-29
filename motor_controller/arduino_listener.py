# opens up a pi serial port and you can read and write from it
import serial
import time
import sys
import redis

# constants
BAUDRATE = 9600

def clear():
	ser.write(b'\x00\x00\x00\x00\x00\x00') #this clears the state machine on the arduino
	time.sleep(1)

def write(motor, direc, pwm):
	mssg_arr = b'P'
	mssg_arr += b'\x01' if motor is 1 else b'\x02'
	mssg_arr += b'\x01' if direc else b'\x00'
	mssg_arr += (b'%02x' % pwm).decode('hex')
	ser.write(mssg_arr)
	time.sleep(0.02)

def read():
	mssg = ser.readline()
	if mssg:
		a = mssg[0]
		encoder_left = mssg[1:12]
		encoder_right = mssg[12:23]
		robot_xmm = mssg[23:34]
		robot_ymm = mssg[34:45]
		robot_theta_deg = mssg[45:56]
		return True, encoder_left, encoder_right, robot_xmm, robot_ymm, robot_theta_deg
	else:
		return False, -999, -999, 0, 0, 0

def motor_encoder_redis(encoder_left, encoder_right, robot_xmm, robot_ymm, robot_theta_deg):
	r.set('encoder_left', encoder_left)
	r.set('encoder_right', encoder_right)
	r.set('robot_xmm', robot_xmm)
	r.set('robot_ymm', robot_ymm)
	r.set('robot_theta_deg', robot_theta_deg)

# main
arduino_port = sys.argv[1]
print "Opening Arduino port at: ", arduino_port
ser = serial.Serial(arduino_port, BAUDRATE, timeout=1)

r = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)
# print "Opening Redis client at: ", r.host, " port: ", r.port

try:
	while True:
		time.sleep(0.05)
		success, encoder_left, encoder_right, robot_xmm, robot_ymm, robot_theta_deg = read()
		if success:
			motor_encoder_redis(encoder_left, encoder_right, robot_xmm, robot_ymm, robot_theta_deg)
except KeyboardInterrupt:
	pass
finally:
	ser.close()
