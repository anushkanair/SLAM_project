import redis

r = redis.StrictRedis(host='localhost', port=6379, db=0)

r.set("motor_left_pwm", "10")
PWM = r.get("motor_left_pwm")
print int(PWM)
