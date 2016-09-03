#include <Arduino.h>
#include <assert.h>
#include "controller.h"

uint16_t Controller::readAdc (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    return analogRead(this->adcArduinoMap(ichan)); 
}

uint16_t Controller::readSwitchedAdc (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 

    float voltage; 

    // step down the gain until we get a voltage reading that isn't too close to the rail. 
    // should probably be replaced with some apriori knowledge of the voltage, based on reading from the uC adc inputs. 

    this->readAdc (ichan); 

    this->gain.setGain (10); 
    voltage = adc.readVoltage();
    if (adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    this->gain.setGain (5); 
    voltage = adc.readVoltage();
    if (adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    this->gain.setGain (2); 
    voltage = adc.readVoltage();
    if (adc.maxVoltage() - voltage < 0.1) {
        return voltage; 
    }

    this->gain.setGain (1); 
    voltage = adc.readVoltage();
    return voltage; 
    
}

void Controller::writeDac (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 
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

uint8_t Controller::adcArduinoMap (uint8_t ichan)
{
    assert (ichan > 0); 
    assert (ichan < NUM_ADC_CHANNELS+1); 
    // ain0=pa0
    // ain2=[b-9
    // ain3=pb09
    //     ain4=pa04
    //     ain5=pa05

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
