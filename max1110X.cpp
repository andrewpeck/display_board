#include <Arduino.h>
#include <SPI.h>
#include <assert.h>

#include "max1110X.h"

Max1110X::Max1110X (uint8_t cs_pin, uint8_t resolution)
{
    this->config.cs_pin     = cs_pin;
    this->config.resolution = resolution;

    pinMode(this->config.cs_pin, OUTPUT);
    digitalWrite(this->config.cs_pin, 1);
}

uint16_t Max1110X::readADC ()
{
    digitalWrite(this->config.cs_pin, LOW);

    uint8_t b0 = SPI.transfer(0x00);
    uint8_t b1 = SPI.transfer(0x00);
    uint8_t b2 = SPI.transfer(0x00);

    // dummy use of b2 to surpress warnings
    b1 = b1 | (b2&0); 


    digitalWrite(this->config.cs_pin, HIGH);

    return (0x3ff & ((b1<<8) | (b0<<0)));
}

float Max1110X::readVoltage ()
{
    return ( this->config.vref * ( readADC() / maxCounts() ));
}

uint16_t Max1110X::maxCounts()
{
    return ((1<<this->config.resolution)-1); 
}

float Max1110X::maxVoltage()
{
    return (this->config.vref * maxCounts());
}
