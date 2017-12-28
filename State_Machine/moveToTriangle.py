from transitions import *
import numpy as np
import numpy.linalg as linalg
from scipy.optimize import fsolve
import time
import redis

class States():
    Rotating = 0
    Translating = 1
    Stopped = 2

COUNTER_CLOCKWISE = 1
CLOCKWISE = 0
FORWARD = 1
REVERSE = 0

ALPHA_THRESHOLD = 2
MOVE_THRESHOLD = 150
ROBOT_TRI_DISTANCE = 650

TF_CAMERA_TO_BASE_ROT = np.array([[1,0,0], [0,0,1], [0,-1,0]])
TF_CAMERA_TO_BASE_TRANS = np.array([0, 193.67, 164.55])

# triangle size
TRI_L12 = 0.95*146.0
TRI_L23 = 0.95*146.0
TRI_L13 = 0.95*87.0

states = ['Rotating', 'Translating', 'Stopped']

class Actions(object):
    def __init__(self):
        self.state = States.Stopped

    class driver_model(object):

        def __init__(self, actions_data):
            self.currentlocationX = 0
            self.currentlocationY = 0
            self.targetlocationX = 0
            self.targetlocationY = 0
            self.angle_to_rotate = 0
            self.distance_to_translate = 0
            self.beta = 0
            self.p = 0
            self.gamma = 0
            self.red = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)
            self.robot_x = 0
            self.robot_y = 0
            self.robot_theta = 0
            self.update_robot_pose()
            self.pre_robot_x = self.robot_x
            self.pre_robot_y = self.robot_y
            self.pre_robot_theta = self.robot_theta
            # triangle co-ordinates
            self.triangle_vec_scales = np.array([800, 700, 800])

    #Conditions
        def desired_pos_reached(self):
            print linalg.norm([self.robot_x - self.pre_robot_x, self.robot_y - self.pre_robot_y]), abs(self.p)
            if linalg.norm([self.robot_x - self.pre_robot_x, self.robot_y - self.pre_robot_y]) > 0.5*abs(self.p):
                return True
            else:
                return False

        def desired_pos_not_reached(self):
            return (not self.desired_pos_reached())

        def desired_pos_changed(self):
            if abs(self.p) < MOVE_THRESHOLD:
                return False
            else:
                return True
            

        def triangle_aligned(self):
            alpha = self.angle_triangle_robot()
            if abs(alpha) > ALPHA_THRESHOLD:
                return False
            else:
                return True

        def triangle_not_aligned(self):
            return (not self.triangle_aligned()) 

        def no_condition(self):
            return True

        def compute_time_translation(self):
            return abs(self.distance_to_translate)/84.0

        def compute_time_rotation(self):
            return abs(self.angle_to_rotate)/40.0

        def update_robot_pose(self):
            self.robot_x = int(self.red.get('robot_xmm'))
            self.robot_y = int(self.red.get('robot_ymm'))
            self.robot_theta = int(self.red.get('robot_theta_deg'))

        def translate(self):
            translation_period = self.compute_time_translation()
            direction = self.direction_of_translation()
            if direction == FORWARD:
                self.red.set('motor_left_speed', 1)
                self.red.set('motor_right_speed', 1)
                time.sleep(translation_period)
            elif direction == REVERSE:
                self.red.set('motor_left_speed', -1)
                self.red.set('motor_right_speed', -1)
                time.sleep(translation_period)
            self.red.set('motor_left_speed', 0)
            self.red.set('motor_right_speed', 0)
            time.sleep(0.1)
            self.update_robot_pose()

        def rotate(self):
            rotation_period = self.compute_time_rotation()
            direction = self.direction_of_rotation()
            if direction == COUNTER_CLOCKWISE:
                self.red.set('motor_left_speed', -1)
                self.red.set('motor_right_speed', 1)
                time.sleep(rotation_period)
            elif direction == CLOCKWISE:
                self.red.set('motor_left_speed', 1)
                self.red.set('motor_right_speed', -1)
                time.sleep(rotation_period)
            self.red.set('motor_left_speed', 0)
            self.red.set('motor_right_speed', 0)
            time.sleep(0.1)
            self.update_robot_pose()

        def get_new_location(self):
            self.pre_robot_x = self.robot_x
            self.pre_robot_y = self.robot_y
            self.pre_robot_theta = self.robot_theta

            # get triangle location
            centroid, normal = self.get_triangle_transform()
            ideal_position = centroid + normal*ROBOT_TRI_DISTANCE
            ideal_position[2] = 0
            print centroid, normal, ideal_position
            self.beta = 180.0/np.pi*np.arctan2(ideal_position[0], ideal_position[1])
            self.p = linalg.norm(ideal_position)
            self.gamma = 180.0/np.pi*np.arccos(np.dot(-normal, ideal_position)/linalg.norm(ideal_position))
            print 180.0/np.pi*np.arctan2(centroid[0], centroid[1])
            if 180.0/np.pi*np.arctan2(centroid[0], centroid[1]) < self.beta:
                self.gamma = -self.gamma
            print "New location: ", machine.model.beta, machine.model.p, machine.model.gamma

        def time_out(self):
            return True

        def set_beta(self):
            self.angle_to_rotate = self.beta

        def set_gamma(self):
            self.angle_to_rotate = self.gamma

        def set_alpha(self):
            self.angle_to_rotate = self.angle_triangle_robot()

        def set_p(self):
            self.distance_to_translate = self.p

        def angle_triangle_robot(self):
            vertex_1 = self.red.get('triangle_vertex1')
            vertex_2 = self.red.get('triangle_vertex2')
            vertex_3 = self.red.get('triangle_vertex3')
            X1, Y1, Z1 = [float(r) for r in vertex_1.split(' ')]
            X2, Y2, Z2 = [float(r) for r in vertex_2.split(' ')]
            X3, Y3, Z3 = [float(r) for r in vertex_3.split(' ')]
            x_centroid = (X1 + X2 + X3)/3
            return (180.0/np.pi) * np.arctan2(x_centroid, 1)

        #computes the direction the robot should rotate based on the sign of the angle
        def direction_of_rotation(self):
            if self.angle_to_rotate < 0:
                return COUNTER_CLOCKWISE
            else:
                return  CLOCKWISE

        def direction_of_translation(self):
            if self.distance_to_translate > 0:
                return FORWARD
            else:
                return REVERSE

        def get_triangle_transform(self):
            vertex_1 = self.red.get('triangle_vertex1')
            vertex_2 = self.red.get('triangle_vertex2')
            vertex_3 = self.red.get('triangle_vertex3')
            P1 = np.array([float(r) for r in vertex_1.split(' ')])
            P2 = np.array([float(r) for r in vertex_2.split(' ')])
            P3 = np.array([float(r) for r in vertex_3.split(' ')])
            def distance_func(hvec):
                h1 = hvec[0]
                h2 = hvec[1]
                h3 = hvec[2]
                return np.array([
                    linalg.norm(h1*P1 - h2*P2)**2 - TRI_L12**2,
                    linalg.norm(h3*P3 - h2*P2)**2 - TRI_L23**2,
                    linalg.norm(h1*P1 - h3*P3)**2 - TRI_L13**2
                ])
            self.triangle_vec_scales = fsolve(distance_func, self.triangle_vec_scales)
            print self.triangle_vec_scales
            #TODO: check if error is small enough
            centroid = 1.0/3.0*(self.triangle_vec_scales[0]*P1 + self.triangle_vec_scales[1]*P2 + self.triangle_vec_scales[2]*P3)
            normal = np.cross(
                self.triangle_vec_scales[0]*P1 - self.triangle_vec_scales[1]*P2,
                self.triangle_vec_scales[2]*P3 - self.triangle_vec_scales[1]*P2
            )
            normal = normal/linalg.norm(normal)
            return np.dot(TF_CAMERA_TO_BASE_ROT, centroid) + TF_CAMERA_TO_BASE_TRANS, np.dot(TF_CAMERA_TO_BASE_ROT, normal)

        def stopRobot(self):
            self.red.set('motor_left_speed', 0)
            self.red.set('motor_right_speed', 0)
            time.sleep(0.1)
            self.update_robot_pose()

transitions = [
#Stopped to Rotating
{ 'trigger': 'advance',
  'conditions' : 'desired_pos_changed',
  'source' : 'Stopped',
  'dest' : 'Rotating',
  'before' : ['set_beta', 'rotate']
},
#Translating to Rotating
{ 'trigger': 'advance',
  'source' : 'Translating',
  'dest' : 'Rotating',
  'before' : ['set_gamma', 'rotate']
},
#Rotating to Translating
{ 'trigger': 'advance',
  'conditions' : ['desired_pos_not_reached'],
  'source' : 'Rotating',
  'dest' : 'Translating',
  'before' : ['set_p', 'translate']
},
#Rotating to Stopped
{ 'trigger': 'advance',
  'conditions' : ["desired_pos_reached", 'triangle_aligned'],
  'source' : 'Rotating',
  'dest' : 'Stopped',
  'before' : 'get_new_location'
},
#Rotating to Rotating
{ 'trigger': 'advance',
  'conditions' : ['desired_pos_reached', 'triangle_not_aligned'],
  'source' : 'Rotating',
  'dest' : 'Rotating',
  'before' : ['set_alpha', 'rotate']
}
]


# initialize state machine
machine = Machine(model=Actions.driver_model(Actions()),
                  states=states,
                  transitions=transitions,
                  auto_transitions=False,
                  initial='Stopped')
# run state machine
machine.model.get_new_location()

try:
    while True:
        print machine.model.state
        machine.model.advance()
        time.sleep(2.0)
except KeyboardInterrupt:
    machine.model.stopRobot()
