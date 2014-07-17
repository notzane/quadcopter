// {XXX/XXX/XXX/XXX/}

#include <Servo.h>

Servo brushless0;
Servo brushless1;
Servo brushless2;
Servo brushless3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  brushless0.attach(6);
  brushless1.attach(9);
  brushless2.attach(10);
  brushless3.attach(11);
  
  brushless0.write(40);
  brushless1.write(40);
  brushless2.write(40);
  brushless3.write(40);
}

void loop() {
  // put your main code here, to run repeatedly: 
  String input = "BLAH!";
  Serial.println("Reading input...");
  input = read_input();
  Serial.print("Input read: ");
  Serial.println(input);
  int motor0 = motorValue(input, 0);
  int motor1 = motorValue(input, 1);
  int motor2 = motorValue(input, 2);
  int motor3 = motorValue(input, 3);
  
  Serial.println("motors");
  Serial.println(motor0);
  Serial.println(motor1);
  Serial.println(motor2);
  Serial.println(motor3);
  
  brushless0.write(motor0);
  brushless1.write(motor1);
  brushless2.write(motor2);
  brushless3.write(motor3);
}

String read_input() {
  int index = 0;
  char data[19];
  
  while (Serial.available() <= 0 || Serial.read() != '{'){}
  delay(12);
  while(Serial.available()>0) {
    char input = Serial.read();
//    Serial.println("char="+input);
    if (input == '}'){break;}
    data[index] = input;
    index ++;
    delay(4);
  }
  data[index] = '\0';
  Serial.print("data=");
  Serial.print(data);
  Serial.println("");
  return String(data);
}

int motorValue(String inputData, int motorNumber) {
  int placeHolder = -1;
  
  switch (motorNumber) {
    case 0:
      placeHolder = 0;
      break;
    case 1:
      placeHolder = 4;
      break;
    case 2:
      placeHolder = 8;
      break;
    case 3:
      placeHolder = 12;
      break;
  }
  String val = inputData.substring(placeHolder, placeHolder+3);
  return val.toInt();
  Serial.println(val.toInt());
}
   
