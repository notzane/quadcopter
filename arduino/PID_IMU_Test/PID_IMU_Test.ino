
/*
 * Ethan and Zane
 * 8/12/14
 * Arduino Quadcopter IMU Test
 */
 
//Includes
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <PID_v1.h>
#include <Servo.h>

MPU6050 mpu;

int pitchVal0;
int pitchVal1;
int pitchVal2;
int pitchVal3;

int finalVal0;
int finalVal1;
int finalVal2;
int finalVal3;

int motor0;
int motor1;
int motor2;
int motor3;

// Yaw/pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector       
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

double pSetpoint, pInput, pOutput;
double rSetpoint, rInput, rOutput;

PID pitchPID(&pInput, &pOutput, &pSetpoint, 4, 0, 0, DIRECT);
PID rollPID(&rInput, &rOutput, &rSetpoint, 4, 0, 0, DIRECT);

Servo brushless0;
Servo brushless1;
Servo brushless2;
Servo brushless3;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    brushless0.attach(5);
    brushless1.attach(6);
    brushless2.attach(10);
    brushless3.attach(11);
  
    brushless0.write(40);
    brushless1.write(40);
    brushless2.write(40);
    brushless3.write(40);
    
    delay(2000);
    Serial.begin(9600);
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    
    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
    
    pSetpoint = -.23;
    pInput = -.23;
    pitchPID.SetMode(AUTOMATIC);
    pitchPID.SetOutputLimits(-10, 10);
    
    rSetpoint = -0.04;
    rInput = -0.04;
    rollPID.SetMode(AUTOMATIC);
    rollPID.SetOutputLimits(-10, 10);
    
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  while(true){ //loop that we can continue out of
    String input = "";
    
    Serial.print(pInput);
    Serial.print("\t");
    Serial.println(rInput);


    Serial.print(pOutput);
    Serial.print("\t");
    Serial.println(rOutput);
    
    pitchVal0 = motor0 + round(pOutput);
    pitchVal1 = motor1 + round(pOutput);
    pitchVal2 = motor2 - round(pOutput);
    pitchVal3 = motor3 - round(pOutput);
    
    finalVal0 = pitchVal0 - round(rOutput);
    finalVal1 = pitchVal1 + round(rOutput);
    finalVal2 = pitchVal2 + round(rOutput);
    finalVal3 = pitchVal3 - round(rOutput);
     
    brushless0.write(finalVal0);
    //brushless1.write(finalVal1);
    brushless2.write(finalVal2);
    //brushless3.write(finalVal3);
    
    
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //Serial.println(F("FIFO overflow!")); Uncomment if you want it to print overflow

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;


        // display Euler angles in radians
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    }
    pInput = ypr[1];
    pitchPID.Compute();
    rInput = ypr[2];
    rollPID.Compute();
   
    Serial.println("Reading input...");
    input = read_input();
    if(input==""){continue;}

    Serial.print("Input read: ");
    Serial.print(input);
    Serial.println("\t");
    
    motor0 = motorValue(input, 0);
    motor1 = motorValue(input, 1);
    motor2 = motorValue(input, 2);
    motor3 = motorValue(input, 3);
  }
}


String read_input() {
  int index = 0;
  char data[19];
  
  if (Serial.available() <= 0 || Serial.read() != '{'){return "";}
  delay(12);
  while(Serial.available()>0) {
    char input = Serial.read();
    Serial.println("char="+input);
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

  
