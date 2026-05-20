/*
 *
 * @file variables.h
 *
 */

#ifndef VARIABLES_H
#define VARIABLES_H



// Variable to hold our runtime Base Address
uint16_t baseSensorAddress = DEFAULT_BASE_ADDRESS;

// Array of object pointers for discovered chips
PCA9555* pcaModules[MAX_CHIPS] = {nullptr};

// Tracks the actual I2C addresses found during boot scan
uint8_t discoveredAddresses[MAX_CHIPS] = {0};

// Tracks total count of active hardware modules detected
uint8_t totalChipsFound = 0;

// Array to store the 16-bit state maps of all discovered chips
uint16_t lastPinStatesArray[MAX_CHIPS] = {0xFFFF};

// Non-blocking time tracking variable
unsigned long lastPollTime = 0;

#endif
