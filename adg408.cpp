#include <Arduino.h>
#include <assert.h>
#include "adg408.h"

ADG408::ADG408 (uint8_t en_pin, uint8_t a0_pin, uint8_t a1_pin, uint8_t a2_pin)
{
    this->config.en_pin=en_pin;
    this->config.a0_pin=a0_pin;
    this->config.a1_pin=a1_pin;
    this->config.a2_pin=a2_pin;

    pinMode(this->config.en_pin, OUTPUT);
    pinMode(this->config.a0_pin, OUTPUT);
    pinMode(this->config.a1_pin, OUTPUT);
    pinMode(this->config.a2_pin, OUTPUT);

    digitalWrite(this->config.en_pin, 0);
    digitalWrite(this->config.a0_pin, 0);
    digitalWrite(this->config.a1_pin, 0);
    digitalWrite(this->config.a2_pin, 0);
}

void ADG408::selectChannel (uint8_t ichan)
{
    ichan = ichan & 0x7;

    digitalWrite(this->config.a0_pin, (ichan>>0)&0x1);
    digitalWrite(this->config.a1_pin, (ichan>>1)&0x1);
    digitalWrite(this->config.a2_pin, (ichan>>2)&0x1);
}

void ADG408::enable()
{
    digitalWrite(this->config.en_pin, 1);
}

void ADG408::disable()
{
    digitalWrite(this->config.en_pin, 0);
}
