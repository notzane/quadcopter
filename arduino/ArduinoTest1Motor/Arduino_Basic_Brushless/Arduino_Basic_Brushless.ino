//The ESC uses the servo library to determine the speed of the motor
#include <Servo.h>

Servo brushless1;
/*The throttle can go from 0 to 180, just like a servo
reads an angle from 0 to 180*/
int throttle = 40;

void setup() {
  Serial.begin(9600);
//The ESC/motor must be attached to a PWM pin
  brushless1.attach(9);
//This delay may be optional, but it just lets the arduino/ESC settle
  delay(1);
/*
Usually, you need some sort of throttle to control an ESC because you
first have to calibrate it by giving it the range of the throttle.
Since we may not use a potentiometer on the quadcopter, starting the
ESC reading the value of 40 is the perfect value for arming the ESC
without any throttle. 40 is the typical value for all Hobby Works 30A
ESCs, but other ESCs may be different.
*/
  brushless1.write(40);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.println(throttle);
    throttle = Serial.read();
  }
    brushless1.write(throttle);
}
