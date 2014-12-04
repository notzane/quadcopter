/*
 * Ethan and Zane
 * 8/29/14
 * Quadcopter PID IMU v3
 * uses .writeMicroseconds()
 */
 
//Includes
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <PID_v1.h>
#include <Servo.h>

MPU6050 mpu;

//MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

//Orientation/motion vars
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

//Declare PID vars for each axis
double pSetpoint, pInput, pOutput;
double rSetpoint, rInput, rOutput;

//Input vars into PID algorithm
PID pitchPID(&pInput, &pOutput, &pSetpoint, 10, 0, 0, DIRECT);
PID rollPID(&rInput, &rOutput, &rSetpoint, 10, 0, 0, DIRECT);

float pOffset;
float rOffset;
int absOffset;

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
    delay(10);
    brushless0.write(40);
    brushless1.write(40);
    brushless2.write(40);
    brushless3.write(40);
    delay(2000);
    Serial.begin(9600);
    //Join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    TWBR = 24; //400kHz I2C clock (200kHz if CPU is 8MHz)
    
    //Initialize Devices
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
    }
    else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    } 
    //-0.23 in rad
    pSetpoint = -13.18;
    pInput = -13.18;
    pitchPID.SetMode(AUTOMATIC);
    pitchPID.SetOutputLimits(-200, 200);
    //-0.03 in rad
    rSetpoint = -1.72;
    rInput = -1.72;
    rollPID.SetMode(AUTOMATIC);
    rollPID.SetOutputLimits(-200, 200);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    getypr();
    pOffset = getPitchPID();
    rOffset = getRollPID();
    //absOffset was + 40
    absOffset = motorVal() + 1000;

    Serial.println(absOffset);
    

    
//    Serial.println(rOffset);
    
}

void getypr() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {}
    
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    
    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //Serial.println(F("FIFO overflow!")); //Uncomment if you want it to print overflow
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & 0x02) {
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
}

float getPitchPID() {
    pInput = (180/PI)*ypr[1];
    pitchPID.Compute();
    return pOutput;
}

float getRollPID() {
    rInput = (180/PI)*ypr[2];
    rollPID.Compute();
    return rOutput;
}

String read_input() {
    int index = 0;
    char data[10];
    while (Serial.available() <= 0 || Serial.read() != '{') { //actual progrm loop
        getypr();
        getypr();
        pOffset = getPitchPID();
        rOffset = getRollPID();
        Serial.print("r: "); Serial.print(rOffset);
        Serial.print("\t p: "); Serial.println(pOffset);
        
        brushless0.writeMicroseconds(absOffset - pOffset + rOffset);
        //Used to have 16, 16, 0, 6
        brushless1.writeMicroseconds(absOffset - pOffset - rOffset);
        brushless2.writeMicroseconds(absOffset + pOffset - rOffset);
        brushless3.writeMicroseconds(absOffset + pOffset + rOffset);
    }
    delay(5);
    while(Serial.available() > 0) {
        char input = Serial.read();
        //Serial.println("char=" + input);
        if (input == '}') {
            data[index] = '\0';
            return String(data);
        }
        data[index] = input;
        index ++;
        delay(4);
    }
    //data[index] = '\0';
    
    //Serial.print("data=");
    //Serial.print(data);

    //return String(data);
}

int motorVal() {
    String val = read_input().substring(0,3);
    return atoi(val.c_str());
    //Serial.println(val);
}


    
    
        
       

