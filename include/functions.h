/*
 *
 * @file functions.h
 * 
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defines.h"

// Scans entire addressing block and registers every healthy chip found
void scanAndInitAllChips() {
    Serial.println("Scanning I2C bus for all available TCA/PCA9555 modules...");
    totalChipsFound = 0;

    for (uint8_t address = I2C_MIN_ADDRESS; address <= I2C_MAX_ADDRESS; address++) {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("Found hardware instance at I2C address: 0x");
            Serial.println(address, HEX);

            // Dynamically instantiate object inside our structural tracking array
            pcaModules[totalChipsFound] = new PCA9555(address);

            // Initialize the newly provisioned module
            if (pcaModules[totalChipsFound]->begin(INPUT)) {
                discoveredAddresses[totalChipsFound] = address;
                
                uint16_t startAddr = baseSensorAddress + (totalChipsFound * PINS_PER_CHIP);
                uint16_t endAddr = startAddr + 15;
                
                Serial.print("  -> Initialized. Mapping to LocoNet Sensors: ");
                Serial.print(startAddr);
                Serial.print(" through ");
                Serial.println(endAddr);

                totalChipsFound++;
            } else {
                Serial.println("  -> Error: Found device, but initialization routine failed!");
                delete pcaModules[totalChipsFound]; // Free memory block if broken
                pcaModules[totalChipsFound] = nullptr;
            }
        }
    }
    
    Serial.print("Scan complete. Total functional expanders configured: ");
    Serial.println(totalChipsFound);
}




// Manages startup EEPROM loading and data integrity checks
void initializeBaseAddress() {
    EEPROM.get(EEPROM_BASE_ADDR_LOCATION, baseSensorAddress);

    // Validate that loading address block won't bleed past maximum limits
    if (baseSensorAddress == 0 || baseSensorAddress > 3900) {
        Serial.println("EEPROM empty or corrupt. Setting default base address to 1.");
        baseSensorAddress = DEFAULT_BASE_ADDRESS;
        EEPROM.put(EEPROM_BASE_ADDR_LOCATION, baseSensorAddress);
    } else {
        Serial.print("Loaded Base Sensor Address from EEPROM: ");
        Serial.println(baseSensorAddress);
    }
}



// Parses configuration commands using requested toUpperCase format
void checkSerialForConfig() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.startsWith("<") && input.endsWith(">")) {
            String command = input.substring(1, input.length() - 1);
            command.trim();

            String upperCommand = command;
            upperCommand.toUpperCase();

            if (upperCommand.startsWith("A")) {
                String valStr = command.substring(1); 
                valStr.trim(); 
                
                long newAddress = valStr.toInt();

                if (newAddress >= 1 && newAddress <= 3900) {
                    baseSensorAddress = (uint16_t)newAddress;
                    EEPROM.put(EEPROM_BASE_ADDR_LOCATION, baseSensorAddress);
                    
                    Serial.print("SUCCESS: Base address updated to ");
                    Serial.println(baseSensorAddress);
                    Serial.println("Restarting board to rebuild layout configuration mappings...");
                    Serial.flush();
                    
                    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
                        asm volatile ("jmp 0");  
                    #endif
                } else {
                    Serial.println("ERROR: Invalid address range. Max base allowed for multi-mode is 3900.");
                }
            }
        }
    }
}


// Send loconet sensor messsage

void transmitSensorReport(uint16_t address, bool active) {
    Serial.print("address : ");
    Serial.print(address);
    Serial.println(active ? " on" : " off");

    LocoNet.reportSensor(address, active ? 1 : 0);
}



#endif
