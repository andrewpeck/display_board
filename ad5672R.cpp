#include <Arduino.h>
#include <SPI.h>
#include <assert.h>
#include "ad5672R.h"

AD5672R::AD5672R (
            uint8_t gain,
            uint8_t resolution,
            uint8_t gain_select_pin,
            uint8_t sync_pin,
            uint8_t reset_pin,
            float   vref,
            bool    software_ldac
        )
{
    this->config.gain            = gain;
    this->config.resolution      = resolution;
    this->config.vref            = vref;
    this->config.software_ldac   = software_ldac;

    this->config.sync_pin        = sync_pin;
    this->config.gain_select_pin = gain_select_pin;
    this->config.reset_pin       = reset_pin;

    pinMode(this->config.gain_select_pin , OUTPUT);
    pinMode(this->config.sync_pin        , OUTPUT);
    pinMode(this->config.reset_pin       , OUTPUT);

    digitalWrite(this->config.gain_select_pin, 0x1&(gain)); // gain is either 0 or 1 (meaning 1 or 2..)
    digitalWrite(this->config.reset_pin,       0x0);        // startup reset
    digitalWrite(this->config.sync_pin,        0x1);        // startup inactive
}

void AD5672R::setDacValue      (uint8_t channel, uint16_t value)
{
    uint32_t packet = buildPacket(CMD_WRITE_AND_LOAD_DAC, channel, value);
    digitalWrite(this->config.sync_pin, LOW);
    SPI.transfer((packet >> 16) & 0xFF); 
    SPI.transfer((packet >>  8) & 0xFF); 
    SPI.transfer((packet >>  0) & 0xFF); 
    digitalWrite(this->config.sync_pin, HIGH);
}

void AD5672R::setDacVoltage    (uint8_t channel, float voltage)
{
    float gainf = (this->config.gain) ? (2.0f) : (1.0f);
    uint16_t dac_counts = static_cast<uint16_t> ( (voltage * this->config.vref * gainf )/maxDacCounts() );
    setDacValue (channel, dac_counts);
}

uint32_t AD5672R::buildPacket   (uint8_t command, uint8_t address, uint16_t data)
{
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // | Bit    | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15  | 14  | 13  | 12  | 11  | 10  | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // | AD5672 | C3 | C2 | C1 | C0 | A3 | A2 | A1 | A0 | D11 | D10 | D9  | D8  | D7  | D6  | D5 | D4 | D3 | D2 | D1 | D0 | X  | X  | X  | X  |
    // | AD5676 | C3 | C2 | C1 | C0 | A3 | A2 | A1 | A0 | D15 | D14 | D13 | D12 | D11 | D10 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|
    // |        | Command           | Address           | Data                                                            | 16-bit data       |
    // |--------+----+----+----+----+----+----+----+----+-----+-----+-----+-----+-----+-----+----+----+----+----+----+----+----+----+----+----|

    /* pad the right 4 LSBs with zeros for 12 bit DAC version */
    if (this->config.resolution==12)
        data = (0xfff & data) << (4);

    /* construct 24 bit data packet */
    uint32_t packet =
          (0xf    & command) << (20)
        | (0xf    & address) << (16)
        | (0xffff & data   ) << (0);

    return packet; 
}

uint16_t AD5672R::maxDacCounts ()
{
    return ((0x2<<this->config.resolution)-1);
}
