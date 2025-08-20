/*
This is a program to link a FDC1004 to a MSP430 LaunchPad.
Circuit is shown in Getting Started Guide but below are the basics:

FDC1004                         MSP430F5529
GND ----------------------------GND
VDD ----------------------------3.3V
SDA--------^^10kOhms^^----------P3.0
SCL--------^^10kOhms^^----------P3.1

The program will print to the serial monitor the data being read from the FDC1004 sensor via i2c.
This program will turn on the LEDS based on the sensor detecting a higher capacitance.
            Red corresponds to the Left Sensor.
            Green corresponds to the Right Sensor.
            
Measurement Explanation: The measurement is a 24 bit value that is stored in 2 registers.
The first register has the 16 most significant bits of the measurement. The second register
has the least significant 8 bits of the measurement stored as bits [15:8] and then the rest of the 
register is filled with zeros [7:0]. Combine all of the first register and the top half of
the second register to obtain the full 24 bit measurement.

Created by: Reed Kaczmarek
Date: 7/14/2015
*/

#include <Wire.h> // Standard I2C Library
#define LED1 RED_LED // Define a name for the Red LED
#define LED2 GREEN_LED // Define a name for the Green LED
int reg = 80; //Address on FDC1004 for measurement register
//INITIALIZATION
void setup() 
{
  Wire.begin(); // Starts the I2C Connection
  Serial.begin(9600); // Starts the Serial Communication
  pinMode(LED1, OUTPUT); // Set LED as output 
  pinMode(LED2, OUTPUT); // Set LED as output
  digitalWrite(LED1, LOW); // Initialize LED off
  digitalWrite(LED2, LOW); // Initialize LED off
  delay(500); // Wait for setup to complete before running program
}
// MAIN
void loop() // This function will run forever after initialization
{
    byte rb1, rb2, rb3, lb1, lb2, lb3; // vairables to store byte values from FDC1004 (r & l refers to right or left sensor)
  unsigned int lbb1, lbb2, lbb3, rbb1, rbb2, rbb3; // variables to store 16-bit unsigned integers
  
  
  // LEFT SENSOR (MEAS1)
  Wire.beginTransmission(reg); 
  Wire.write(0);  // MSB of MEAS1_MSB REG *On Datasheet: p.16 Table 1 shows the register (0x00) we are accessing
  Wire.write(0);  // LSB of MEAS1_MSB REG *On Datasheet: p.16 Table 1 shows the register (0x00) we are accessing
  Wire.endTransmission();
  Wire.requestFrom(reg, 2); // Ask for 2 bytes to be transmitted
  while(Wire.available()) {
    lb1 = Wire.read(); // Left byte 1 (MSB)
    lb2 = Wire.read(); // Left byte 2 (2nd MSB)
  }
  Wire.beginTransmission(reg);
  Wire.write(1); // MSB of MEAS1_LSB REG *On Datasheet: p.16 Table 1 shows the register (0x01) we are accessing
  Wire.endTransmission();
  Wire.requestFrom(reg, 1); // Ask for 1 byte to be transmitted
  while(Wire.available()) {
    lb3 = Wire.read(); // Left byte 3 (LSB)
  }
  
  // Below are the calculations to take the 24-bit measurement and convert into capacitance for MEAS1
  // * From datasheet: p.16 Section 8.6.1.1 shows the formula (all we need to do is shift the 24-bit value right by 19)
  // ** Shifting by 19 takes a 24 bit number and makes it a 5 bit number with 19 bits after the decimal point
  lbb1 = lb1*256 + lb2; // Puts 16 most significant bits into one integer for left sensor measurement
  lbb2 = lbb1>>11; // Sets lbb2 to the 5 bits that exist before the decimal point
  lbb3 = 0b0000011111111111 & lbb1; // Sets lbb3 to the 11 bits that exist after the decimal point
  Serial.print("LEFT CAP:  ");
  Serial.print(lbb2 + (float)lbb3/2048 +(float)lb3/1048576, 4); // Performs an algorithm to assign bit values and add components
  Serial.print("  pF    ");
  if((lbb2 + (float)lbb3/2048 +(float)lb3/1048576) > 1){
    digitalWrite(LED1, HIGH);
  }else{
    digitalWrite(LED1, LOW);
  }
  
  
  // RIGHT SENSOR (MEAS4)
  Wire.beginTransmission(reg);
  Wire.write(6);  // MSB of MEAS4_MSB REG *On Datasheet: p.16 Table 1 shows the register (0x06) we are accessing
  Wire.write(6);  // LSB of MEAS4_MSB REG *On Datasheet: p.16 Table 1 shows the register (0x06) we are accessing
  Wire.endTransmission();
  Wire.requestFrom(reg, 2); // Ask for 2 bytes to be transmitted
  while(Wire.available()) {
    rb1 = Wire.read(); // Right byte 1 (MSB)
    rb2 = Wire.read(); // Right byte 2 (2nd MSB)
  }
  Wire.beginTransmission(reg);
  Wire.write(7); // MSB of MEAS4_LSB REG *On Datasheet: p.16 Table 1 shows the register (0x07) we are accessing
  Wire.endTransmission();
  Wire.requestFrom(reg, 1); // Ask for 1 byte to be transmitted
  while(Wire.available()) {
    rb3 = Wire.read();  // Right byte 3 (LSB)
  }
  
   // Below are the calculations to take the 24-bit measurement and convert into capacitance for MEAS4
  // * From datasheet: p.16 Section 8.6.1.1 shows the formula (all we need to do is shift the 24-bit value right by 19)
  // ** Shifting by 19 takes a 24 bit number and makes it a 5 bit number with 19 bits after the decimal point
  rbb1 = rb1*256 + rb2; // Puts 16 most significant bits into one int of rbb1
  rbb2 = rbb1>>11; // Sets rbb2 to the 5 bits that are before the decimal point
  rbb3 = 0b0000011111111111 & rbb1; // Sets rbb3 to the 11 bits that are after the decimal point
  Serial.print("RIGHT CAP:  ");
  Serial.print(rbb2 + (float)rbb3/2048 +(float)rb3/1048576, 4); // Performs an algorithm to assign bit values and add
  Serial.println("  pF.");
  if((rbb2 + (float)rbb3/2048 +(float)rb3/1048576) > 1){
    digitalWrite(LED2, HIGH);
  }else{
    digitalWrite(LED2, LOW);
  }
  delay(100); // Pause between sampling from the FDC1004
}
