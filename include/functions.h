/*
 *
 * @file functions.h
 * 
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H


// Scan valid TCA9555 I2C addresses (0x20 - 0x27)
uint8_t getTCA9555Address()
 {
//  Wire.begin();
  uint8_t a = 0;
  for (uint8_t address = I2C_MIN_ADDRESS; address <= I2C_MAX_ADDRESS; address++)
   {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
     {
      Serial.print("TCA9555 found at address: 0x");
      Serial.println(address, HEX);
      a = address;
      break;
     }
   }
  return a;
 }


#endif
