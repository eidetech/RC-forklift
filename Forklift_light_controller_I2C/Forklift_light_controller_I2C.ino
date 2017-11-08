// Wire Slave Receiver
#include <Wire.h>

int x = 0;
boolean warning_light = false;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);  // start serial for output
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {


if (warning_light == true) {
  digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
}else{
  digitalWrite(2, LOW);
}
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.println(x);         // print the integer
  
  
  if (x==1) {
    digitalWrite(3, HIGH);
  }else if (x==2) {
    digitalWrite(3, LOW);
  }else if (x==3) {
    warning_light = true;
  }else if (x==4) {
    warning_light = false;
  }else {
    
  }
  
  
/*
  if (x==1) {
  digitalWrite(2, HIGH);
  }else if (x==2) {
  digitalWrite(13, HIGH);
  }else if (x==3) {
  digitalWrite(2, HIGH);
  digitalWrite(13, HIGH);
  }else if (x==4) {
  digitalWrite(2, LOW);
  }else if (x==5) {
  digitalWrite(13, LOW);
  }else{
  digitalWrite(2, LOW);
  digitalWrite(13, LOW);
  }
  */
}
