//The ESC uses the servo library to determine the speed of the motor
#include <Servo.h>

Servo brushless1;
Servo brushless2;
Servo brushless3;
Servo brushless4;
/*The throttle can go from 0 to 180, just like a servo
reads an angle from 0 to 180*/
int throttle = 40;

void setup() {
  Serial.begin(9600);
//The ESC/motor must be attached to a PWM pin
  brushless1.attach(5);
  brushless2.attach(6);
  brushless3.attach(10);
  brushless4.attach(11);
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
  brushless2.write(40);
  brushless3.write(40);
  brushless4.write(40);
  delay(2000);
}

void loop() {
  if (Serial.available() > 0) {
    throttle = Serial.read();
    Serial.println(throttle);
  }
   
    brushless1.writeMicroseconds(1000);
    brushless2.writeMicroseconds(1000);
    brushless3.writeMicroseconds(1000);
    brushless4.writeMicroseconds(1000);
}
