#include <Arduino.h>
#include <SPI.h>
#include <assert.h>
#include "ad567XR.h"

void AD567XR::initialize (
            uint8_t gain,
            uint8_t gain_select_pin,
            uint8_t sync_pin,
            uint8_t reset_pin,
            float   vref,
            bool    software_ldac,
            uint8_t resolution
        )
{
    this->config.gain            = gain;
    this->config.resolution      = resolution;
    this->config.vref            = vref;
    this->config.software_ldac   = software_ldac;

    this->config.sync_pin        = sync_pin;
    this->config.gain_select_pin = gain_select_pin;
    this->config.reset_pin       = reset_pin;

    pinMode(config.gain_select_pin , OUTPUT);
    pinMode(config.sync_pin        , OUTPUT);
    pinMode(config.reset_pin       , OUTPUT);

    disable();  // startup reset
    deselect(); // startup with cs inactive
    setGain(gain);
}

void AD567XR::setValue      (uint8_t channel, uint16_t value)
{
    /* KEEP THIS HERE */
    SPI.setDataMode(SPI_MODE2);
    /* We want to build the packet before setting chip-select to introduce some small delay... */

    uint32_t packet = buildPacket(CMD_WRITE_AND_LOAD_DAC, channel, value);
    
    select();
    SPI.transfer((packet >> 16) & 0xFF);
    SPI.transfer((packet >>  8) & 0xFF);
    SPI.transfer((packet >>  0) & 0xFF);
    deselect();
}

void AD567XR::setVoltage    (uint8_t channel, float voltage)
{
    float gainf = (this->config.gain) ? (2.0f) : (1.0f);
    uint16_t dac_counts = static_cast<uint16_t> ( (voltage * this->config.vref * gainf )/maxDacCounts() );
    setValue (channel, dac_counts);
}

uint32_t AD567XR::buildPacket   (uint8_t command, uint8_t address, uint16_t data)
{
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // | Bit    | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15  | 14  | 13  | 12  | 11  | 10  | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // | AD567X | C3 | C2 | C1 | C0 | A3 | A2 | A1 | A0 | D11 | D10 | D9  | D8  | D7  | D6  | D5 | D4 | D3 | D2 | D1 | D0 | X  | X  | X  | X  |
    // | AD5676 | C3 | C2 | C1 | C0 | A3 | A2 | A1 | A0 | D15 | D14 | D13 | D12 | D11 | D10 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // |        | Command           | Address           | Data                                                            | 16-bit data       |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|

    /* pad the right 4 LSBs with zeros for 12 bit DAC version */
    if (this->config.resolution==12)
        data = (0xfff & data) << (4);


    // SerialUSB.println(command, HEX);
    // SerialUSB.println(address, HEX);
    // SerialUSB.println(data>>4, DEC);

    /* construct 24 bit data packet */
    uint32_t packet =
          (0xf    & command) << (20)
        | (0xf    & address) << (16)
        | (0xffff & data   ) << (0);

    return packet;
}

uint16_t AD567XR::maxDacCounts ()
{
    return ((0x1<<this->config.resolution)-1);
}

void AD567XR::enable()
{
    digitalWrite(this->config.reset_pin, HIGH);
}

void AD567XR::disable()
{
    digitalWrite(this->config.reset_pin, LOW);
}

void AD567XR::select()
{
    digitalWrite(this->config.sync_pin, LOW);
}

void AD567XR::deselect()
{
    digitalWrite(this->config.sync_pin, HIGH);
}

void AD567XR::setGain(uint8_t gain)
{
    if (gain==2) {
        digitalWrite(this->config.gain_select_pin, HIGH);
    }
    else {
        digitalWrite(this->config.gain_select_pin, LOW);
    }
}
