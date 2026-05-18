/*
 *
 * @file variables.h
 *
 */

#ifndef VARIABLES_H
#define VARIABLES_H


uint8_t i2cAddress = 0x20;

PCA9555 *tca;


// Adress Switch on the pcb
// TODO when pcb designed then set these
//int adrr_switch[4] = { 5, 4, 3, 2 };
uint8_t address = 100;   // Byte to store Address (Only the 4 LSB are used)


// Starting address for your LocoNet sensors
uint8_t startAddress = 1;
uint16_t lastStates = 0xFFFF; // Store last known states of all 16 pins

uint16_t previousMillis = 0;
#endif
