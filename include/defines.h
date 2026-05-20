/*
 *
 * @file defines.h
 * 
 */
#ifndef DEFINES_H
#define DEFINES_H

// LocoNet Pin Configuration
#define LN_RX_PIN 8
#define LN_TX_PIN 6

// EEPROM Memory Mapping
#define EEPROM_BASE_ADDR_LOCATION 0 
#define DEFAULT_BASE_ADDRESS 1      

// Maximum possible PCA9555 chips on a single I2C bus segment (0x20 to 0x27)
#define MAX_CHIPS 8
#define PINS_PER_CHIP 16



#define I2C_MIN_ADDRESS 0x20
#define I2C_MAX_ADDRESS 0x27

#define WAIT_MILLIS_LONG 150
#define WAIT_MILLIS_SHORT 50

#endif