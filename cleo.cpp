#include "cleo.h"
#include <Arduino.h>

CleOCtrl::CleOCtrl (uint8_t reset_pin, uint8_t irq_pin, uint8_t cs_pin)
{
    this->config.reset_pin = reset_pin;
    this->config.irq_pin   = irq_pin;
    this->config.cs_pin    = cs_pin;

//    pinMode(reset_pin , OUTPUT);
    pinMode(irq_pin   , INPUT );
    pinMode(cs_pin    , OUTPUT);

    disable();
    deselect();
}

void CleOCtrl::enable()
{
 //   digitalWrite(this->config.reset_pin, LOW);
}

void CleOCtrl::disable()
{
    //digitalWrite(this->config.reset_pin, HIGH);
}

void CleOCtrl::select()
{
    digitalWrite(this->config.cs_pin, LOW);
}

void CleOCtrl::deselect()
{
    digitalWrite(this->config.cs_pin, HIGH);
}
