#include<Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include<EEPROM.h>
#include<stdlib.h>
#include<AFMotor.h>
#include "motor_driver.h"
#include "command.h"

const boolean VERBOSE = true;

int trackcounter = 1;

//Set L298N module IO Pin
const int ENA = 5;
const int ENB = 6;
const int IN1 = 7;
const int IN2 = 8;
const int IN3 = 9;
const int IN4 = 11;

//Set up Sensors
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);//assign a unique ID to this sensor
float heading = 0;
float headingDegrees = 0;

const int mtl = 2;
int mtlvalue = LOW;

const int headtrig = A0;
const int headecho = A1;
const int righttrig = A2;
const int rightecho = A3;
const int lefttrig = 3;
const int lefttecho = 4;
const int reartrig = 10;
const int rearecho = 12;
const float diserrH = 0;
const float diserrR = 0;
const float diserrL = 0;
const float diserrRR = 0;
float durationh = 0;
float distanceh = 0;
float durationr = 0;
float distancer = 0;
float durationl = 0;
float distancel = 0;
float durationrr = 0;
float distancerr = 0;

//Patching IO for mechanical
const int patch = 13;

//Emergency lightLED
const int led = 1;

//Set state
int s0 = 0;//Start Point, moving forward and scan
int s1 = 1;//Adjust heading direction
int s2 = 2;//Patching and recording the hole
int s3 = 3;//Moving backward and scan
int s4 = 4;//Moving to next track
int s5 = 5;//Emergency stop with LED light-on
int s6 = 6;//Job done
int state = s0;

//Record location
int addr = 0;


void forward(){
  setMotorSpeed(0,210);
  setMotorSpeed(2,210);
  setMotorSpeed(1,210);
  setMotorSpeed(3,210);
  }
void back(){
  setMotorSpeed(0,-210);
  setMotorSpeed(2,-210);
  setMotorSpeed(1,-210);
  setMotorSpeed(3,-210);
  }
void zero_turnright() {
  setMotorSpeed(0,210);
  setMotorSpeed(2,-210);
  setMotorSpeed(1,-210);
  setMotorSpeed(3,210);
}
void zero_turnleft() {
  setMotorSpeed(0,-210);
  setMotorSpeed(2,210);
  setMotorSpeed(1,210);
  setMotorSpeed(3,-210);
}
void slowleft() {
  setMotorSpeed(0,130);
  setMotorSpeed(2,130);
  setMotorSpeed(1,-130);
  setMotorSpeed(3,-130);
}
void slowright() {
  setMotorSpeed(0,-130);
  setMotorSpeed(2,-130);
  setMotorSpeed(1,130);
  setMotorSpeed(3,130);
}
void changelane() {
  setMotorSpeed(0,210);
  setMotorSpeed(2,-210);
  setMotorSpeed(1,-210);
  setMotorSpeed(3,210);
  delay(500);
}
 void STOP(){
    stopMotors();
    }
    

//    The direction of the car's movement
//  ENA   ENB   IN1   IN2   IN3   IN4   Description
//  HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
//  HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
//  HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
//  HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
//  HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
//  HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
//  LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped

//void forward() {
//  digitalWrite(ENA, HIGH); //enable L298N A channel
//  digitalWrite(ENB, HIGH); //enable L298N B channel
//  digitalWrite(IN1, HIGH); //set IN1 hight level
//  digitalWrite(IN2, LOW); //set IN2 low level
//  digitalWrite(IN3, LOW); //set IN3 low level
//  digitalWrite(IN4, HIGH); //set IN4 hight level
//  //  Serial.println("Forward");//send message to serial monitor
//}
//
//void back() {
//  digitalWrite(ENA, HIGH);
//  digitalWrite(ENB, HIGH);
//  digitalWrite(IN1, LOW);
//  digitalWrite(IN2, HIGH);
//  digitalWrite(IN3, HIGH);
//  digitalWrite(IN4, LOW);
//  //  Serial.println("Back");
//}
//
//void zero_turnleft() {
//  digitalWrite(ENA, HIGH);
//  digitalWrite(ENB, HIGH);
//  digitalWrite(IN1, LOW);
//  digitalWrite(IN2, HIGH);
//  digitalWrite(IN3, LOW);
//  digitalWrite(IN4, HIGH);
//  //  Serial.println("Left");
//}
//
//void zero_turnright() {
//  digitalWrite(ENA, HIGH);
//  digitalWrite(ENB, HIGH);
//  digitalWrite(IN1, HIGH);
//  digitalWrite(IN2, LOW);
//  digitalWrite(IN3, HIGH);
//  digitalWrite(IN4, LOW);
//  //  Serial.println("Right");
//}
//
//void STOP() {
//  digitalWrite(ENA, HIGH);
//  digitalWrite(ENB, HIGH);
//  digitalWrite(IN1, LOW);
//  digitalWrite(IN2, LOW);
//  digitalWrite(IN3, LOW);
//  digitalWrite(IN4, LOW);
//  //  Serial.println("STOP");
//}
//
////slow speed mode
//void slowforward() {
//  analogWrite(ENA, 125); //enable L298N A channel
//  analogWrite(ENB, 125); //enable L298N B channel
//  digitalWrite(IN1, HIGH); //set IN1 hight level
//  digitalWrite(IN2, LOW); //set IN2 low level
//  digitalWrite(IN3, LOW); //set IN3 low level
//  digitalWrite(IN4, HIGH); //set IN4 hight level
//  //  Serial.println("Forward");//send message to serial monitor
//}
//
//void slowback() {
//  analogWrite(ENA, 125);
//  analogWrite(ENB, 125);
//  digitalWrite(IN1, LOW);
//  digitalWrite(IN2, HIGH);
//  digitalWrite(IN3, HIGH);
//  digitalWrite(IN4, LOW);
//  //  Serial.println("Back");
//}
//
//void slowleft() {
//  analogWrite(ENA, 125);
//  analogWrite(ENB, 200);
//  digitalWrite(IN1, LOW);
//  digitalWrite(IN2, HIGH);
//  digitalWrite(IN3, LOW);
//  digitalWrite(IN4, HIGH);
//  //  Serial.println("Left");
//}
//
//void slowright() {
//  analogWrite(ENA, 200);
//  analogWrite(ENB, 125);
//  digitalWrite(IN1, HIGH);
//  digitalWrite(IN2, LOW);
//  digitalWrite(IN3, HIGH);
//  digitalWrite(IN4, LOW);
//  //  Serial.println("Right");
//}
//
//void changelane() {
//  zero_turnright();
//  delay(500);
//  forward();
//  delay(500);
//  zero_turnleft();
//  delay(500);
//}

void setup() {
  Serial.begin(9600);//open serial and set the baudrate
  Wire.begin();
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(mtl, INPUT);
  pinMode(headtrig, OUTPUT);
  pinMode(headecho, INPUT);
  pinMode(righttrig, OUTPUT);
  pinMode(rightecho, INPUT);
  pinMode(lefttrig, OUTPUT);
  pinMode(lefttecho, INPUT);
  pinMode(reartrig, OUTPUT);
  pinMode(rearecho, INPUT);
  pinMode(led, OUTPUT);

}
//Main program
void loop() {

  //****Compass***
  sensors_event_t event;
  mag.getEvent(&event);
  heading = atan2(event.magnetic.y, event.magnetic.x);
  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180 / M_PI;

  boolean north = (headingDegrees < 180.5 && headingDegrees > 179.5); //heading north
  boolean east = (headingDegrees < 90.5 && headingDegrees > 89.5); //heading east
  boolean overnorth180 = (headingDegrees > 180.5);
  boolean lessnorth180 = (headingDegrees < 179.5);

  //***metal detector***
  mtlvalue = digitalRead(mtl);
  boolean metal = (mtlvalue == HIGH);

  //***Ultrasonic***
  //Head distance
  digitalWrite(headtrig, LOW);
  delayMicroseconds(2);
  digitalWrite(headtrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(headtrig, LOW);
  durationh = pulseIn(headecho, HIGH);
  distanceh = ((durationh / 2) * 0.0344 + diserrH) * 0.393701;
  //Right distance
  digitalWrite(righttrig, LOW);
  delayMicroseconds(2);
  digitalWrite(righttrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(righttrig, LOW);
  durationr = pulseIn(rightecho, HIGH);
  distancer = ((durationr / 2) * 0.0344 + diserrR) * 0.393701;
  //Left distance
  digitalWrite(lefttrig, LOW);
  delayMicroseconds(2);
  digitalWrite(lefttrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(lefttrig, LOW);
  durationl = pulseIn(lefttecho, HIGH);
  distancel = ((durationl / 2) * 0.0344 + diserrL) * 0.393701;
  //Rear distance
  digitalWrite(reartrig, LOW);
  delayMicroseconds(2);
  digitalWrite(reartrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(reartrig, LOW);
  durationrr = pulseIn(rearecho, HIGH);
  distancerr = ((durationrr / 2) * 0.0344 + diserrRR ) * 0.393701;
  boolean lasttrack = (trackcounter == 7);
  boolean oddtrack = (0 == trackcounter % 2);
  boolean headend = (distanceh <= 5.9 );//front end of the wall,less than 5.9 inch
  boolean rearend = (distancerr <= 5.9);//Rear end of the wall, less than 5.9 inch
  boolean rightend = (distancer <= 5.9 ); // Right side end of the wall, less than 5.9 inch
  boolean leftend = (distancel <= 5.9 ); // Left side end of the wall, less than 5.9 inch


  switch (state) {
    case 0://forward
      forward();//forward moving scan
      if (headend == true && rearend == false && leftend == false && rightend == true && north == true && metal == false && lasttrack == true) {
        state = s6;
      }//job done
      else if (headend == false && rearend == false && north == false && metal == false) {
        state = s1;
      }//adjusting the head direction}
      else if (metal == true) {
        state = s2;
      }//patching&recording the hole}
      else if (headend == false && rearend == true && leftend == false && rightend == false && north == true && metal == false) {
        state = s4;
      }//track scan finished, change the lane}
      else if (headend == false && rearend == false && leftend == false && rightend == false && north == true && metal == false && lasttrack == false && oddtrack == true) {
        state = s0;
      }//keep moving forward and scan
      else {
        state = s5;
      }
      break;

    case 1://Adjust heading direction
      if (headend == false && rearend == false && north == false && metal == false && overnorth180 == true) {
        zero_turnright();
        state = s1;
      }
      else if (headend == false && rearend == false && north == false && metal == false && lessnorth180 == true) {
        zero_turnleft();
        state = s1;
      }
      else if (north == true && metal == false && oddtrack == true) {
        state = s0;
      }//continue forward scan
      else if (north == true && metal == true) {
        state = s2;
      }//patching&recording the hole}
      else if (headend == false && rearend == true && rightend == false && north == true && metal == false) {
        state = s4;
      }//change lane
      else if (headend == true && rearend == false && rightend == false && north == true && metal == false) {
        state = s4;
      }//change lane
      else if (north == true && metal == false && oddtrack == false) {
        state = s3;
      }//continue backward scan
      else if (headend == true && rearend == false && leftend == false && rightend == true && metal == false && north == true && lasttrack == true) {
        state = s6;
      }//job done
      else {
        state = s5;
      }//err
      break;

    case 2:
      if (metal == true) {
        STOP();
        digitalWrite(patch, HIGH);
        delay(500);
        digitalWrite(patch, LOW);
        int X = trackcounter;
        char Y = (distancel - 6) / 12;
        EEPROM.write(addr, X);
        addr = addr + 1;
        EEPROM.write(addr, Y);
        addr = addr + 1 ;
        if (addr == 512)
          addr = 0;
        delay(100);
        //***Record the location***
        state = s2;
      }
      else if ( headend == false && rearend == false && leftend == false && rightend == false && north == true && metal == false && oddtrack == true) {
        state = s0;
      }//continue forward scan
      else if (headend == false && rearend == false && leftend == false && rightend == false && north == true && metal == false && oddtrack == false) {
        state = s3;
      }//continue backward scan
      else if (headend == false && rearend == true && rightend == false && north == true && metal == false) {
        state = s4;
      }//change lane
      else if (headend == true && rearend == false && rightend == false && north == true && metal == false) {
        state = s4;
      }//change lane
      else if (headend == false && rearend == false && north == false && metal == false) {
        state = s1;
      }//adjusting heading direction
      else if (headend == true && rearend == false && leftend == false && rightend == true && north == true && metal == false && lasttrack == true) {
        state = s6;
      }//job done
      else {
        state = s5;
      }//err
      break;

    case 3:
      back();//backward scan
      if (headend == false && rearend == false && leftend == false && rightend == false && north == true && metal == false && oddtrack == false) {
        state = s3;
      }//backward scan
      else if (headend == false && rearend == false && north == false && metal == false) {
        state = s1;
      }//adjusting heading direction
      else if (metal == true) {
        state = s2;
      }//patching and recording the hole
      else if (headend == false && rearend == true && leftend == false && rightend == false && north == true && metal == false) {
        state = s4;
      }//change lane
      else {
        state = s5;
      }
      break;

    case 4:
      changelane();
      trackcounter++;
      if (metal == true) {
        state = s2;
      }
      else if (headend == false && rearend == true && north == true && metal == false && oddtrack == true) {
        state = s0;
      }
      else if (headend == true && rearend == false && north == true && metal == false && oddtrack == false) {
        state = s3;
      }
      else if (north == false && metal == false) {
        state = s1;
      }
      else {
        state = s5;
      }
      break;

    case 5:
      digitalWrite(led, HIGH);
      STOP();
      state = s5;
      break;

    case 6:
      STOP();
      state = s6;
      break;
  }

}
