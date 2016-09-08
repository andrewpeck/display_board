#include <Arduino.h>
#include <assert.h>
#include "controller.h"

uint16_t Controller::readArduinoAdc (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    return analogRead(this->arduinoAdcPinswap(arduinoAnalogPin(ichan))); 
}

uint8_t arduinoAnalogPin (uint8_t ichan)
{
    switch (ichan) {
        case 0x1: return PIN_A0;
        case 0x2: return PIN_A1;
        case 0x3: return PIN_A2;
        case 0x4: return PIN_A3;
        case 0x5: return PIN_A4;
        case 0x6: return PIN_A5;
        case 0x7: return PIN_A6;
        case 0x8: return PIN_A7;
        default:  return 0x0;
    }
}

uint16_t Controller::readSwitchedAdc (uint8_t ichan, uint8_t gain) 
{
    setSwitchGain(gain); 
    readSwitchedAdc(ichan); 
}

uint16_t Controller::readSwitchedAdc (uint8_t ichan) 
{
    return adc.readVoltage();
}

uint16_t Controller::autoReadSwitchedAdc (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    float voltage; 

    // step down the gain until we get a voltage reading that isn't too close to the rail. 
    // should probably be replaced with some apriori knowledge of the voltage, based on reading from the uC adc inputs. 

    // TODO 
    // this->readArduinoAdc (ichan); 

    setSwitchGain(10); 
    voltage = readSwitchedAdc(); 
    if (this->adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    setSwitchGain(5); 
    voltage = readSwitchedAdc(); 
    if (this->adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    setSwitchGain(2); 
    voltage = readSwitchedAdc(); 
    if (this->adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    setSwitchGain(1); 
    voltage = readSwitchedAdc(); 
    return voltage; 
    
}

void Controller::writeDac (uint8_t ichan, uint16_t value)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 
    this->dac.setDacValue(ichan, value); 
}

void Controller::setSwitchGain (uint8_t gain)
{
    this->gain.setGain(gain); 
}

void Controller::setDDSFrequency (uint32_t freq)
{
    this->dds.setFrequency(freq); 
}

uint8_t Controller::adcPinswap (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    // Converts from channel inputs to enumerated analog inputs in the Microcontroller schematic
    // Mapping from digital_logic.SchDoc
    switch (ichan) {
        case 0x1: return 0x8;
        case 0x2: return 0x7;
        case 0x3: return 0x6;
        case 0x4: return 0x5;
        case 0x5: return 0x4;
        case 0x6: return 0x3;
        case 0x7: return 0x2;
        case 0x8: return 0x1;
        default:  return 0x0;
    }
}

uint8_t Controller::arduinoAdcPinswap (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    // in "Digital Logic", the order of inputs is reversed; 
    // need to unreverse it in software


    switch (ichan) {
        case 0x1: return 0x8;
        case 0x2: return 0x7;
        case 0x3: return 0x6;
        case 0x4: return 0x5;
        case 0x5: return 0x4;
        case 0x6: return 0x3;
        case 0x7: return 0x2;
        case 0x8: return 0x1;
        default:  return 0x0;
    }
}

uint8_t Controller::dacPinswap (uint8_t ichan)
{
    // Convert from DAC outputs (determined by DAC hardware) to schematic channel outputs 
    // Derived from dac.SchDoc

    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    switch (ichan) {
        case 0x1: return 0x4;
        case 0x2: return 0x3;
        case 0x3: return 0x2;
        case 0x4: return 0x1;
        case 0x5: return 0x6;
        case 0x6: return 0x5;
        case 0x7: return 0x8;
        case 0x8: return 0x7;
        default:  return 0x0;
    }
}

uint8_t Controller::switchPinswap (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    // Map pinswapping done on the ADG408 inputs which multiplex the Amplifier/ADC input
    switch (ichan) {
        case 0x1: return 0x4;
        case 0x2: return 0x3;
        case 0x3: return 0x3;
        case 0x4: return 0x1;
        case 0x5: return 0x5;
        case 0x6: return 0x6;
        case 0x7: return 0x7;
        case 0x8: return 0x8;
        default:  return 0x0;
    }
}
