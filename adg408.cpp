#include <Arduino.h>
#include <assert.h>
#include "adg408.h"

ADG408::ADG408 (uint8_t en_pin, uint8_t a0_pin, uint8_t a1_pin, uint8_t a2_pin)
{
    this->config.en_pin=en_pin;
    this->config.a0_pin=a0_pin;
    this->config.a1_pin=a1_pin;
    this->config.a2_pin=a2_pin;

    digitalWrite(en_pin, HIGH);

    pinMode(en_pin, OUTPUT);
    pinMode(a0_pin, OUTPUT);
    pinMode(a1_pin, OUTPUT);
    pinMode(a2_pin, OUTPUT);

    enable();
    setChannel(0);
}

void ADG408::setChannel (uint8_t ichan)
{
    ichan = ichan & 0x7;

    digitalWrite(this->config.a0_pin, (ichan>>0)&0x1);
    digitalWrite(this->config.a1_pin, (ichan>>1)&0x1);
    digitalWrite(this->config.a2_pin, (ichan>>2)&0x1);
}

void ADG408::enable()
{
    pinMode     (this->config.en_pin, OUTPUT);
    digitalWrite(this->config.en_pin, HIGH);
}

void ADG408::disable()
{
    pinMode     (this->config.en_pin, OUTPUT);
    digitalWrite(this->config.en_pin, LOW);
}
