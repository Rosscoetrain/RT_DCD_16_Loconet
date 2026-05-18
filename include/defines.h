/*
 *
 * @file defines.h
 * 
 */
#ifndef DEFINES_H
#define DEFINES_H

#define USE_DS104

#ifdef USE_DS104
#define LOCONET_TX_PIN 6
#else
#define LOCONET_TX_PIN 12
#endif

#define I2C_MIN_ADDRESS 0x20
#define I2C_MAX_ADDRESS 0x27

#define WAIT_MILLIS 100

#endif