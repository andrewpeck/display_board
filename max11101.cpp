#include <Arduino.h>
#include <SPI.h>
#include <assert.h>

#include "max11101.h"

Max11101::Max11101 (uint8_t cs_pin)
{
    this->config.cs_pin=cs_pin;

    pinMode(this->config.cs_pin, OUTPUT);
    digitalWrite(this->config.cs_pin, 1);
}

uint16_t Max11101::readADC ()
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

float Max11101::readVoltage ()
{
    return ( this->config.vref * ( readADC() / maxCounts() ));
}

uint16_t Max11101::maxCounts()
{
    return ((1<<14)-1); 
}

float Max11101::maxVoltage()
{
    return (this->config.vref * maxCounts());
}

