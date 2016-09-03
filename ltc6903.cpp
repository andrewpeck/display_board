#include <Arduino.h>
#include <SPI.h>
#include <assert.h>
#include "ltc6903.h" 

//----------------------------------------------------------------------------------------------------------------------
// Public
//----------------------------------------------------------------------------------------------------------------------

LTC6903::LTC6903 (uint8_t sen_pin, uint8_t oe_pin, bool clk_enable, bool clk_bar_enable)
{
    this->config.sen_pin = sen_pin;
    this->config.oe_pin  = oe_pin;
    this->config.clk_enable  = clk_enable;
    this->config.clk_bar_enable  = clk_bar_enable;

    this->config.cnf = generateCnf (clk_enable, clk_bar_enable);

    pinMode(this->config.oe_pin,  OUTPUT);
    pinMode(this->config.sen_pin, OUTPUT);

    digitalWrite(this->config.oe_pin,  0);
    digitalWrite(this->config.sen_pin, 1);
}

void LTC6903::enable()
{
    digitalWrite(this->config.oe_pin, 1);
}

void LTC6903::disable()
{
    digitalWrite(this->config.oe_pin, 0);
}

uint16_t LTC6903::buildWord (uint8_t oct, uint8_t dac)
{
    uint16_t word; 
    word  = this->config.cnf;
    word |= (0x3ff & dac) << 3;
    word |= (  0xf & oct) << 12;

    return word; 
}

void LTC6903::setFrequency(uint8_t oct, uint8_t dac)
{
    // we should enable this here... no reason to be writing frequency if it is off.
    digitalWrite(this->config.oe_pin, 1);

    uint16_t word = buildWord(oct, dac);
    digitalWrite(this->config.sen_pin, 0);
    SPI.transfer (0xff & (word>>8));
    SPI.transfer (0xff & (word>>0));
    digitalWrite(this->config.sen_pin, 1);
}

//----------------------------------------------------------------------------------------------------------------------
// Private
//----------------------------------------------------------------------------------------------------------------------

uint8_t LTC6903::generateCnf (bool clk_enable, bool clk_bar_enable)
{
    uint8_t cnf_pre = (clk_enable<<1) | clk_bar_enable;

    switch (cnf_pre) {
        case 0x0: return (0x3);
        case 0x1: return (0x1);
        case 0x2: return (0x2);
        case 0x3: return (0x0);
        default : return ( -1);
    }
}

uint8_t LTC6903::generateDac (uint32_t frequency, uint8_t oct)
{
    uint64_t exp;
    switch (oct) {
        case 0x0: exp=2078ul * 0x1<<(10+0 ); break;
        case 0x1: exp=2078ul * 0x1<<(10+1 ); break;
        case 0x2: exp=2078ul * 0x1<<(10+2 ); break;
        case 0x3: exp=2078ul * 0x1<<(10+3 ); break;
        case 0x4: exp=2078ul * 0x1<<(10+4 ); break;
        case 0x5: exp=2078ul * 0x1<<(10+5 ); break;
        case 0x6: exp=2078ul * 0x1<<(10+6 ); break;
        case 0x7: exp=2078ul * 0x1<<(10+7 ); break;
        case 0x8: exp=2078ul * 0x1<<(10+8 ); break;
        case 0x9: exp=2078ul * 0x1<<(10+9 ); break;
        case 0xa: exp=2078ul * 0x1<<(10+10); break;
        case 0xb: exp=2078ul * 0x1<<(10+11); break;
        case 0xc: exp=2078ul * 0x1<<(10+12); break;
        case 0xd: exp=2078ul * 0x1<<(10+13); break;
        case 0xe: exp=2078ul * 0x1<<(10+14); break;
        case 0xf: exp=2078ul * 0x1<<(10+15); break;
        default : exp=0;
    }
    uint8_t dac = 2048 - exp/frequency;

    return dac; 
}

uint8_t LTC6903::generateOct (uint32_t frequency)
{
    if      (frequency <     1039) return  (-1);
    else if (frequency <     2076) return (0x0);
    else if (frequency <     4152) return (0x1);
    else if (frequency <     8304) return (0x2);
    else if (frequency <    16610) return (0x3);
    else if (frequency <    33220) return (0x4);
    else if (frequency <    66430) return (0x5);
    else if (frequency <   132900) return (0x6);
    else if (frequency <   265700) return (0x7);
    else if (frequency <   531400) return (0x8);
    else if (frequency <  1063000) return (0x9);
    else if (frequency <  2126000) return (0xa);
    else if (frequency <  4252000) return (0xb);
    else if (frequency <  8503000) return (0xc);
    else if (frequency < 17010000) return (0xd);
    else if (frequency < 34010000) return (0xe);
    else if (frequency < 68030000) return (0xf);
    else                           return  (-1);
}

void    LTC6903::setFrequency(uint32_t frequency)
{
    uint8_t oct = generateOct (frequency);

    if (oct<16) {
        uint8_t dac = generateDac (frequency, oct);
        setFrequency(oct,dac);
    }
}

