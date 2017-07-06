#include <wiringPi.h>
#include <softPwm.h>

int main (void) {
  wiringPiSetup () ;
  softPwmCreate (7,0, 100) ;
  softPwmWrite (7, 75) ;

  for (;;) delay (1000) ;
}
