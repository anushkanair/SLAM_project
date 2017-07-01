from transitions import *
import numpy

class States():
    Stopped = 0
    Moving = 1
    ForceStop = 2
    Charging = 3
    Calibrating = 4

states = ['Stopped', 'Moving', "ForceStop", 'Charging', 'Calibrating', 'Searching_Artag']

class NavigationMode():
    Artag = 0
    Position = 1

class Actions(object):
    def __init__(self):
        self.state = States.Calibrating
        self.nav_mode = NavigationMode.Position

    class driver_model(object):


        def __init__(self, actions_data):
            self.actions_data = actions_data
            self.calibrated = False
            self.currentlocationX = 0
            self.currentlocationY = 0
            self.targetlocationX = 0
            self.targetlocationY = 0
            self.chargerlocationX = 0
            self.chargerlocationY = 0
            self.artag_locationX = 0
            self.artag_locationY = 0

        # conditions

        def is_done_calibrating(self):
            calibrated = False
            print("updateStateMachine: Calibrating : Do not disturb machine while calibrating is in progress.")
            #rotating idea dealing with the machine
            return True

        def desired_pos_reached(self):
            if self.actions_data.nav_mode is not NavigationMode.Position:
                return False
            if (abs(self.currentlocationX - self.targetlocationX) <= 0.1 and abs(self.currentlocationY - self.targetlocationY <= 0.1)):
                return True
            else:
                return True

        def desired_artag_reached(self):
            if self.actions_data.nav_mode is not NavigationMode.Artag:
                return False
            if (abs(self.currentlocationX - self.artag_locationX) <= 0.1 and abs(self.currentlocationY - self.artag_locationY <= 0.1)):
                return True
            else:
                return True

        def desired_pos_changed(self):
            return True

        def no_condition(self):
            return True

        def computed_path_complete(self):
            self.actions_data.nav_mode = NavigationMode.Position
            return True
            #is path done.

        def computed_artag_path_complete(self):
            self.actions_data.nav_mode = NavigationMode.Artag
            return True
            #is path done.

        def is_charging(self):
            return True

        def fully_charged(self):
            #if (battery.power == 100):
            return True

        def battery_low(self):
            return True
            #if (battery.power < certain value):
            #   gotocharger()

        def wall_close(self):
            #if (object.distance < 0.1):
            return True

        def is_ARTAG_visible(self):
            #search for artag (if else)
            return True

        def is_ARTAG_invisible(self):
            #search for artag (if else)
            return not self.is_ARTAG_visible()

        #actions

        def go_to_charger(self):
            self.targetlocationX == self.chargerlocationX
            self.targetlocationY == self.chargerlocationY

        def run_motors(self):
            return True

        def stop_motors(self):
            return True
            #motor.setSpeed(0)

        def rotate(self, degrees):
            return True
            #motor.rotate(degrees)

        def get_current_location(self):
            return True
            #ask the SLAM where the robot is. SLAM.get_current_location

        def search_for_artag(self):
            #rotate 360
            return True

transitions = [
#Calibrating to Stopped
{ 'trigger': 'advance',
  'conditions' : 'is_done_calibrating',
  'source' : 'Calibrating',
  'dest' : 'Stopped',
  'before' : 'stop_motors'
},
#Stopped to Calibrating
{ 'trigger' : 'advance',
  'conditions' : 'is_ARTAG_visible',
  'source' : 'Stopped',
  'dest' : 'Calibrating',
  'before' : 'rotate' and 'get_current_location'
  #can we do both of these in before?
},
# Stopped to Moving
{ 'trigger' : 'new_destination',
  'conditions' : 'computed_path_complete',
  'source' : 'Stopped',
  'dest' : 'Moving',
  'before' : 'run_motors'
},
# Stopped to Moving
{ 'trigger' : 'new_artag',
  'conditions' : 'computed_artag_path_complete',
  'source' : 'Stopped',
  'dest' : 'Moving',
  'before' : 'run_motors'
},
# Moving to Stopped
{ 'trigger' : 'advance',
  'conditions' : 'desired_pos_reached',
  'source' : 'Moving',
  'dest' : 'Stopped',
  'before' : 'stop_motors'
},
#Moving to Searching_Artag
{ 'trigger' : 'advance',
  'conditions' : 'desired_artag_reached',
  'source' : 'Moving',
  'dest' : 'Searching_Artag',
  'before' : 'search_for_artag'
},
#Searching_Artag to Stopped
{ 'trigger' : 'advance',
  'conditions' : 'is_ARTAG_visible',
  'source' : 'Searching_Artag',
  'dest' : 'Stopped',
  'before' : 'stop_motors'
},
#Searching_Artag to ForceStop
{ 'trigger' : 'advance',
  'conditions' : 'is_ARTAG_invisible',
  'source' : 'Searching_Artag',
  'dest' : 'ForceStop',
  'before' : 'stop_motors'
},
#ForceStop to Calibrating
{ 'trigger' : 'user_input',
  'conditions' : 'no_condition',
  'source' : 'ForceStop',
  'dest' : 'Calibrating',
  'before' : 'ask_for_input'
 },
 #Moving to Charging
 { 'trigger' : 'battery_low',
   'conditions' : 'is_charging',
   'source' : 'Moving',
   'dest' : 'Charging',
   'before' : 'go_to_charger'
 },
 #Stopped to Charging
 { 'trigger' : 'battery_low',
   'conditions' : 'is_charging',
   'source' : 'Stopped',
   'dest' : 'Charging',
   'before' : 'go_to_charger'
 },
 #Charging to Stopped
 { 'trigger' : 'advance',
   'conditions' : 'fully_charged',
   'source' : 'Charging',
   'dest' : 'Stopped'
 }
]

Marvin = Actions()
machine = Machine(model=Actions.driver_model(Marvin),
                  states=states,
                  transitions=transitions,
                  auto_transitions=False,
                  initial='Calibrating')

# #calibrating to stopped
# print machine.model.state
# machine.model.advance()
# #stopped to charging
# print machine.model.state
# machine.model.battery_low()
# #charging to stopped
# print machine.model.state
# machine.model.advance()
# #stopped to moving
# print machine.model.state
# machine.model.new_destination()
# #moving to charging
# print machine.model.state
# machine.model.battery_low()
# #charging to stopped
# print machine.model.state
# machine.model.advance()
# #stopped to moving
# print machine.model.state
# machine.model.new_artag()
# #moving to Searching_Artag
# print machine.model.state
# machine.model.advance()
# print machine.model.state
# machine.model.advance()
# print machine.model.state
