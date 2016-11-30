#include <Time.h>
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>
#include <TinyGPS.h>

#define EEPROM_addrs  1
#define SW_ACC  7
#define SW_TOL  3
#define SW_GPS  4
#define SW_CAM  5
#define VI_ACC  A1
#define TX_GPS  13
#define RX_GPS  12
#define RX_OL  11
#define TX_OL  10
#define LED  13
#define I_HUM  A3
#define temp_add 72
#define GPS_THRESHOLD 3

float read_temp(int *val, int *frac);

void setup() {
  // put your setup code here, to run once:
   // Initialize i2c
  Wire.begin();
    // initialize the serial communication:
  Serial.begin(4800);
  pinMode(SW_TOL, OUTPUT);
  pinMode(SW_ACC, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);


  if(Serial)
  Serial.println("PotBot v_2.5");

  digitalWrite(SW_TOL, HIGH);
  digitalWrite(SW_ACC, LOW);
//  digitalWrite(A4, LOW);
//  digitalWrite(A5, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
int val, frac;
read_temp(&val, &frac);
delay(1000);
}


float read_temp(int *val, int *frac){
  float temperature;
//  digitalWrite(SW_TOL, HIGH);
  digitalWrite(LED, HIGH);
  // Serial.print("TEMP ON \n");  //ZZZ delete
  delay(10);
  // Serial.print("begin tx \n");  //ZZZ delete
  Wire.beginTransmission(temp_add);
  // send configuration
  // Serial.print("send config \n");  //ZZZ delete
  Wire.write(0xAC);
  Wire.write(B00001111); // 12 bit resolution, pol, oneshot
  Wire.endTransmission();
  delay(10);
 
  // begin convert
  // Serial.print("begin convert \n");  //ZZZ delete
  Wire.beginTransmission(temp_add); 
  Wire.write(0x51);
  Wire.endTransmission(); 
  delay(20);
 
  // wait until converting is done
  // Serial.print("Wait for convertion \n");  //ZZZ delete
  byte conf = 0;
  while ( conf & B1000000 != B10000000 ) {
    delay(20);
    Wire.beginTransmission(temp_add); 
    Wire.write(0xAC); 
    Wire.endTransmission();
    conf = Wire.read();
  }

 // Serial.print("Ask temperature \n");  //ZZZ delete
  // ask for the temerature 
  Wire.beginTransmission(temp_add); 
  Wire.write(0xAA); 
  Wire.endTransmission();

 // Serial.print("Read bytes \n");  //ZZZ delete
  // request 2 bytes
  Wire.requestFrom(temp_add, 2);
  // read first byte
 
  *val = Wire.read();
  
  // read second byte
  *frac = Wire.read();
  
  if (*frac>100)
    temperature=(float)*frac/1000;
  else if (*frac>10)
    temperature=(float)*frac/100;
  else
    temperature=(float)*frac/10;
  temperature+=(float)*val;
  Serial.print("\n  Temperature float: ");
  Serial.println(temperature,3);
  *frac = 100 * (*frac & 0xF0 )/ 256;
  if ( *frac < 10 ) {

  }

  
  Serial.print("\n");

  delay(50);
//  digitalWrite(SW_TOL, LOW);
  digitalWrite(LED, LOW);
  return temperature;
}
