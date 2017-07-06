import redis

r = redis.StrictRedis(host='localhost', port=6379, db=0)

def run_motors_redis(value, value2):
    r.set("motor_left_pwm", value)
    r.set("motor_right_pwm", value2)

//run_motors_redis(10, 10)
