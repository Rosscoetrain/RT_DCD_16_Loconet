/*
 *
 *
 *
 *
 * Loconet TCA/PCA9555 interface
 *
 * Using RT_DCD_16 or RT_PCA9555 boards interfaces loconet via a
 *
 * Model Rail Enginering DS104 board.
 *
 * This checks for all TCA/PCA9555 boards and adds them to the sensor list.
 *
 * Base Loconet address is configurable via serial monitor using <A address> command.
 *
 * Base Loconet address can be 1 to 3900 this allows for 8 TCA/PCA9555 boards with sixteen inputs on each.
 *
 *
 *
 *
 *
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <TCA9555.h>
#include <LocoNet.h>
#include <EEPROM.h>

#include "defines.h"
#include "variables.h"
#include "functions.h"

/*
void setup() {
    Serial.begin(57600);
    Wire.begin();
    Serial.println("\n--- Multi-Chip LocoNet Sensor Node Starting ---");

    // Load or initialize the base sensor address from EEPROM
    initializeBaseAddress();

    // Scan for and initialize every connected PCA9555 module
    scanAndInitAllChips();

    if (totalChipsFound == 0) {
        Serial.println("CRITICAL ERROR: No PCA9555 chips found in range 0x20-0x27!");
        while (1);
    }

    // Initialize LocoNet on standard pins
    LocoNet.init(LN_TX_PIN);
    Serial.println("LocoNet interface initialized.");

    // Loop through all chips and all pins to broadcast initial startup states
    Serial.println("Sending initial state reports for all discovered inputs...");
    for (uint8_t chipIdx = 0; chipIdx < totalChipsFound; chipIdx++) {
        // Read baseline and cache it
        lastPinStatesArray[chipIdx] = pcaModules[chipIdx]->read16();

        // Calculate address offset for this specific chip's block of 16 sensors
        uint16_t chipBaseAddress = baseSensorAddress + (chipIdx * PINS_PER_CHIP);

        for (int pinIdx = 0; pinIdx < PINS_PER_CHIP; pinIdx++) {
            bool currentBit = bitRead(lastPinStatesArray[chipIdx], pinIdx);
            uint16_t sensorAddress = chipBaseAddress + pinIdx;
            bool isActive = (currentBit == LOW); // LOW means grounded/occupied

            transmitSensorReport(sensorAddress, isActive);
            delay(150); // Safe pacing delay for startup burst traffic
        }
    }

    Serial.println("System Ready. Monitoring all connected expansion inputs.");
    Serial.println("To change base address, send '<A address>' or '<a address>' (e.g., <A 100>).");
}

void loop() {
    // Check if the user is trying to update configuration via Serial
    checkSerialForConfig();

    // Sequentially poll each discovered chip
    for (uint8_t chipIdx = 0; chipIdx < totalChipsFound; chipIdx++) {
        uint16_t currentPinStates = pcaModules[chipIdx]->read16();

        // Check if any pins on this specific chip have toggled state
        if (currentPinStates != lastPinStatesArray[chipIdx]) {

            // Calculate starting LocoNet address for this chip block
            uint16_t chipBaseAddress = baseSensorAddress + (chipIdx * PINS_PER_CHIP);

            for (int pinIdx = 0; pinIdx < PINS_PER_CHIP; pinIdx++) {
                bool currentBit = bitRead(currentPinStates, pinIdx);
                bool lastBit = bitRead(lastPinStatesArray[chipIdx], pinIdx);

                if (currentBit != lastBit) {
                    uint16_t sensorAddress = chipBaseAddress + pinIdx;
                    bool isActive = (currentBit == LOW);

                    transmitSensorReport(sensorAddress, isActive);

                    Serial.print("Chip @ 0x");
                    Serial.print(discoveredAddresses[chipIdx], HEX);
                    Serial.print(" Pin ");
                    Serial.print(pinIdx);
                    Serial.print(" -> Address ");
                    Serial.print(sensorAddress);
                    Serial.println(isActive ? " ACTIVE" : " INACTIVE");
                }
            }
            // Update individual cache tracking for this chip index
            lastPinStatesArray[chipIdx] = currentPinStates;
            delay(50); // Small local debounce buffer spacing
        }
    }

    // Process incoming LocoNet traffic to maintain network health
    lnMsg* inMsg = LocoNet.receive();
    if (inMsg) {
        // Optional tracking logic can go here
    }
}
*/

void setup() {
    Serial.begin(57600);
    Wire.begin();
    Serial.println("\n--- Non-Blocking Multi-Chip LocoNet Sensor Node Starting ---");

    // Load or initialize the base sensor address from EEPROM
    initializeBaseAddress();

    // Scan for and initialize every connected PCA9555 module
    scanAndInitAllChips();

    if (totalChipsFound == 0) {
        Serial.println("CRITICAL ERROR: No PCA9555 chips found in range 0x20-0x27!");
        while (1); 
    }

    // Initialize LocoNet on standard pins
    LocoNet.init(LN_TX_PIN);
    Serial.println("LocoNet interface initialized.");

    // Loop through all chips and all pins to broadcast initial startup states
    Serial.println("Sending initial state reports for all discovered inputs...");
    for (uint8_t chipIdx = 0; chipIdx < totalChipsFound; chipIdx++) {
        // Read baseline and cache it
        lastPinStatesArray[chipIdx] = pcaModules[chipIdx]->read16();

        // Calculate address offset for this specific chip's block of 16 sensors
        uint16_t chipBaseAddress = baseSensorAddress + (chipIdx * PINS_PER_CHIP);

        for (int pinIdx = 0; pinIdx < PINS_PER_CHIP; pinIdx++) {
            bool currentBit = bitRead(lastPinStatesArray[chipIdx], pinIdx);
            uint16_t sensorAddress = chipBaseAddress + pinIdx;
            bool isActive = (currentBit == LOW); // LOW means grounded/occupied

            transmitSensorReport(sensorAddress, isActive);
            
            // Startup network buffer pacing can safely keep a small delay, 
            // as setup() only executes once at boot time.
            delay(WAIT_MILLIS_LONG); 
        }
    }

    Serial.println("System Ready. Monitoring all connected expansion inputs.");
    Serial.println("To change base address, send '<A address>' or '<a address>' (e.g., <A 100>).");
    
    // Seed our tracking timestamp right before opening up the main loop execution
    lastPollTime = millis();
}

void loop() {
    // Check if the user is trying to update configuration via Serial (Completely non-blocking)
    checkSerialForConfig();

    // Capture the snapshot of the current microcontroller runtime clock
    unsigned long currentMillis = millis();

    // Non-blocking wrapper: Only query I2C bus at set intervals
    if (currentMillis - lastPollTime >= WAIT_MILLIS_SHORT) {
        lastPollTime = currentMillis; // Advance timer stamp

        // Sequentially poll each discovered chip
        for (uint8_t chipIdx = 0; chipIdx < totalChipsFound; chipIdx++) {
            uint16_t currentPinStates = pcaModules[chipIdx]->read16();

            // Check if any pins on this specific chip have toggled state
            if (currentPinStates != lastPinStatesArray[chipIdx]) {
                
                // Calculate starting LocoNet address for this chip block
                uint16_t chipBaseAddress = baseSensorAddress + (chipIdx * PINS_PER_CHIP);

                for (int pinIdx = 0; pinIdx < PINS_PER_CHIP; pinIdx++) {
                    bool currentBit = bitRead(currentPinStates, pinIdx);
                    bool lastBit = bitRead(lastPinStatesArray[chipIdx], pinIdx);

                    if (currentBit != lastBit) {
                        uint16_t sensorAddress = chipBaseAddress + pinIdx;
                        bool isActive = (currentBit == LOW);

                        transmitSensorReport(sensorAddress, isActive);

                        Serial.print("Chip @ 0x");
                        Serial.print(discoveredAddresses[chipIdx], HEX);
                        Serial.print(" Pin ");
                        Serial.print(pinIdx);
                        Serial.print(" -> Address ");
                        Serial.print(sensorAddress);
                        Serial.println(isActive ? " ACTIVE" : " INACTIVE");
                    }
                }
                // Update individual cache tracking for this chip index
                lastPinStatesArray[chipIdx] = currentPinStates;
            }
        }
    }

    // Process incoming LocoNet traffic to maintain network health.
    // Because delay() is gone, this function executes tens of thousands of times 
    // per second, completely eliminating packet dropping on busy LocoNet layouts.
    lnMsg* inMsg = LocoNet.receive();
    if (inMsg) {
        // Optional tracking logic can go here
    }
}



