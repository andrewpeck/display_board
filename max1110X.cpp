#include <Arduino.h>
#include <SPI.h>
#include <assert.h>

#include "max1110X.h"

uint16_t Max1110X::readAdc ()
{
    SPI.setDataMode(SPI_MODE0);
    select();

    uint8_t b0 = SPI.transfer(0x00);
    uint8_t b1 = SPI.transfer(0x00);
    uint8_t b2 = SPI.transfer(0x00);

    deselect();

    uint16_t value =bitmask() & ((b1<<8) | (b2<<0));

//    SerialUSB.print("\n");
//    SerialUSB.print("b0  ");
//    SerialUSB.print(b0);
//    SerialUSB.print("  b1 ");
//    SerialUSB.print(b1);
//    SerialUSB.print("  b2 ");
//    SerialUSB.print(b2);
//    // dummy use of b0 to surpress warnings
//    b1 = b1 | (b0&0); 

//    SerialUSB.print(" value ");
//    SerialUSB.print(value);

//    SerialUSB.print("\n");


    return (value);
}

float Max1110X::readVoltage ()
{
    return ( this->config.vref * ( readAdc() / maxCounts() ));
}

uint16_t Max1110X::maxCounts()
{
    return ((1<<this->config.resolution)-1); 
}

float Max1110X::maxVoltage()
{
    return (this->config.vref * maxCounts());
}

void Max1110X::select()
{
    //digitalWrite(this->config.cs_pin, LOW);
    digitalWrite(11, LOW);
}

void Max1110X::deselect()
{
    //digitalWrite(this->config.cs_pin, HIGH);
    digitalWrite(11, HIGH);
}

uint16_t Max1110X::bitmask()
{
    return (this->config.resolution==14 ? 0x3fff : 0xffff);
}

void Max1110X::initialize(uint8_t cs_pin, uint8_t resolution)
{
    this->config.cs_pin     = cs_pin;
    this->config.resolution = resolution;

    //SerialUSB.println("cs_pin");
    //SerialUSB.println(cs_pin, DEC);

    pinMode(cs_pin, OUTPUT);

    deselect();
}
