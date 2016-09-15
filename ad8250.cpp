#include <Arduino.h>
#include <assert.h>
#include "ad8250.h" 

AD8250::AD8250 (uint8_t a0_pin, uint8_t a1_pin)
{
    this->config.a0_pin = a0_pin;
    this->config.a1_pin = a1_pin;

    pinMode(a0_pin, OUTPUT);
    pinMode(a1_pin, OUTPUT);

    digitalWrite(a0_pin, HIGH);
    digitalWrite(a1_pin, HIGH);

    setGain(1);
}

void AD8250::setGain(uint8_t gain)
{
    uint8_t digi;
    switch (gain) {
        case 1:  digi = 0x0; break;
        case 2:  digi = 0x1; break;
        case 5:  digi = 0x2; break;
        case 10: digi = 0x3; break;
        default: digi = 0x3; break;
    }
    digitalWrite(this->config.a0_pin, (digi >> 0) & 0x1);
    digitalWrite(this->config.a1_pin, (digi >> 1) & 0x1);
}
