import redis
import time

r = redis.StrictRedis(host='localhost', port=6379, db=0)

while True:
    r.set('motor_left_speed', 0)
    r.set('motor_right_speed', 0)
    x = raw_input('Enter Key')

    if x == "w" or x == 'W':
        r.set('motor_left_speed', 1)
        r.set('motor_right_speed', 1)
        time.sleep(0.5)
    elif x == 'S' or x == 's':
        r.set('motor_left_speed', -1)
        r.set('motor_right_speed', -1)
        time.sleep(0.5)
    elif x == 'D' or x == 'd':
        r.set('motor_left_speed', 1)
        r.set('motor_right_speed', -1)
        time.sleep(0.5)
    elif x == 'A' or x == 'a':
        r.set('motor_left_speed', -1)
        r.set('motor_right_speed', 1)
        time.sleep(0.5)


