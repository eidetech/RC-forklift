/*
Sketch for the RC forklift
*/

//Motor driver library
#include <SparkFun_TB6612.h>
//Servo library
#include <Servo.h>
//Wire library to send commands for lights
#include <Wire.h>
//SPI library
#include <SPI.h>
//nRF24L01 libraries
#include <nRF24L01.h>
#include <RF24.h>
//printf.h sketch for printing out nRF24l01 information
#include "printf.h"

//Motor driver pins
#define AIN1 2
#define BIN1 3
#define AIN2 4
#define BIN2 10
#define PWMA 5
#define PWMB 6
#define STBY 0

//Pipe address
const uint64_t pipeIn =  0xB00B1E5000LL;

//Pins for the radio
RF24 radio(7, 8);

/* Struct to hold variables from receiver
The size of this struct should not exceed 32 bytes */
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte dial1;
  byte dial2;
  byte switch1;
  byte switch2;
};

//Data object
MyData data;

//Variables used to reverse the rotation of motors
const int offsetA = 1;
const int offsetB = -1;

// Initializing motors.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

//Servo object
Servo steering;

//Last value sent to the light controller Arduino
int lastValue1 = 0;
int lastValue2 = 0;
int lastValue3 = 0;
int lastValue4 = 0;

//Motor speed dividers
int lSpeed = 2;

void setup()
{
   Serial.begin(9600);
   Wire.begin();
   printf_begin();

  // Set up radio module
  radio.begin();
  radio.setDataRate(RF24_250KBPS); // Both endpoints must have this set the same
  radio.setAutoAck(false);
  radio.openReadingPipe(1,pipeIn);
  radio.startListening();
  radio.printDetails();

  //Attach servo
  steering.attach(9);

  //Initialize motors, so they don't spin around.
  motor1.drive(0);
  motor2.drive(0);
}

void loop() {

  if ( radio.available() ) {
  radio.read(&data, sizeof(MyData));

  //DRIVE
  //FORWARD
  if (data.throttle > 125) {

  int mapd_forward = map(data.throttle, 125, 255, 0, 255);
  motor1.drive(mapd_forward);

    delay(10);
  }else if (data.throttle >= 125){
  motor1.drive(0);
  }
 delay(10);

  //BACKWARD
  if (data.throttle < 118) {
  int mapd_backward = map(data.throttle, 118, 0, 0, -255);
  motor1.drive(mapd_backward);
  }

  //FORKLIFT ARMS
  //LIFT
  if(data.pitch > 140) {

    motor2.drive(data.pitch/lSpeed);

  }else if (data.pitch >= 140){
   motor2.drive(0);
  }else{
  motor2.drive(0);
  }
  //LOWER
 if (data.pitch < 135) {
  int mapped = map(data.pitch, 135, 0, 0, -255);

  motor2.drive(mapped/lSpeed);
  }

  //SERVO
  //Map steering input to fit servo
  int degree = map(data.roll, 0, 255, 91, 69);
  //Write the mapped data to the servo
  steering.write(degree);

  }else{
  //Reset truck to zero pos.
  motor1.drive(0);
  motor2.drive(0);
  steering.write(88);
  }

  //LED controller
  Wire.beginTransmission(8); // transmit to device #8
  Serial.print("Switch1: ");
  Serial.println(data.switch1);

  Serial.print("Switch2: ");
  Serial.println(data.switch2);

  //Button 1
  if (lastValue1 != data.switch1) {

  if (data.switch1 == 1){
  Wire.write(1);
  lastValue1 = data.switch1;

  }else if (data.switch1 == 0) {
   Wire.write(2);
   lastValue1 = data.switch1;
  }
  }

  //Button 2
  if (lastValue2 != data.switch2) {

  if (data.switch2 == 1){
  Wire.write(3);
  lastValue2 = data.switch2;

  }else if (data.switch2 == 0) {
   Wire.write(4);
   lastValue2 = data.switch2;
  }
  }

  //Pot 1
  if (lastValue3 != data.dial1) {
  Wire.write(data.dial1);
  lastValue3 = data.dial1;
  }

  //Pot 2
  if (lastValue4 != data.dial2) {
  Wire.write(data.dial2);
  lastValue4 = data.dial2;
  }

  Wire.endTransmission();
}
