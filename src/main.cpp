/*
 *
 * Copyright 2026 Ross Scanlon @ RosscoeTrain
 *
 * 
 * Loconet producer to check TCA9555 for sensor inputs
 * 
 * 
 */

#include <Arduino.h>

#include <EEPROM.h>
#include <LocoNet.h>
//#include <TCA9555.h>
#include <PCA9555.h>
#include <Wire.h>

#include "defines.h"
#include "variables.h"
#include "functions.h"


void setup() {
  Serial.begin(115200);

  Wire.begin();

  i2cAddress = getTCA9555Address();

  if (i2cAddress)
   {
    Serial.print("Hello TCA9555 at 0x");
    Serial.println(i2cAddress, HEX);
   }
  else
   {
    Serial.println("No TCA9555 found");
    while (1)
     {
     }
   }   

  // Initialize LocoNet (Pins 7 and 8 are standard for most interfaces)
  LocoNet.init(LOCONET_TX_PIN);
  
  // Initialize TCA9555
//  tca = new TCA9555(i2cAddress);
  tca = new PCA9555(i2cAddress);

//  if (!tca.begin()) {
//    // Handle initialization error if needed
//  }

/*
  // Read Address Pins
  //  TODO work out where these are connected.

  for (int i = 0; i < 4; i++) {
   pinMode(adrr_switch[i], INPUT_PULLUP);
   boolean reading = !digitalRead(adrr_switch[i]);
   bitWrite(address, i, reading);
  }
*/

  address++;

  if(EEPROM.read(0) == address) {
    Serial.println("Address matches EEPROM");
  } else {
    Serial.println("Address Changed");
    EEPROM.write(0, address);
  }

  startAddress = address;

  Serial.print("start address : ");
  Serial.println(startAddress);

  // Set all 16 pins of PCA9555 as INPUT
//  tca->pinMode16(0xFFFF);

  uint8_t io_config_and_pull_up[] = {
    0xFF,  // Configure port0 as INPUT
    0xFF,  // Configure port1 as INPUT
  };

  tca->config(io_config_and_pull_up);  //  Port0 as INPUT, port1 as INPUT    


  Serial.println("Init done");

  Serial.println(lastStates, BIN);
}

void loop() {
  uint16_t currentMillis = millis();

  // 1. Handle incoming LocoNet messages (required by library)
  lnMsg *LnPacket = LocoNet.receive();
  if (LnPacket) {
    LocoNet.processSwitchSensorMessage(LnPacket);
  }

  // 2. Read all 16 pins from TCA9555
//  uint16_t currentStates = tca->read16();

  uint8_t input_0 = tca->input(0);
  uint8_t input_1 = tca->input(1);
  uint16_t currentStates = (input_1 << 8) | input_0;



  // 3. Check for state changes
  if (currentStates != lastStates)
   {
    Serial.println(lastStates, BIN);
    Serial.println(currentStates, BIN);
    for (int i = 0; i < 16; i++) {
      bool currentState = bitRead(currentStates, i);
      bool lastState = bitRead(lastStates, i);

      if (currentState != lastState) {
        // Sensor state changed! Send LocoNet message.
        // LocoNet sensor values: 1 for Active/Occupied, 0 for Inactive/Free
        // Note: Logic depends on your hardware (active-low vs active-high)
        int sensorValue = currentState ? 0 : 1;

        Serial.print("Sensor : ");
        Serial.println(i + 1);

        LocoNet.reportSensor(startAddress + i, sensorValue);
      }
    }
    lastStates = currentStates; // Update memory
  }

  while (currentMillis < previousMillis + WAIT_MILLIS)
   {}
  previousMillis = currentMillis;
}
