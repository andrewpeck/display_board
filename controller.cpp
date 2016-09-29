#include <Arduino.h>
#include <assert.h>
#include "debug_print.h"
#include "controller.h"

//----------------------------------------------------------------------------------------------------------------------
// Built-in ADC
//----------------------------------------------------------------------------------------------------------------------

uint16_t Controller::readArduinoAdc (uint8_t ichan)
{
    // if (ichan <0 || ichan > NUM_ADC_CHANNELS)
    //     return (-1);

    //debug_println ("readArduinoAdc assertion passed"/;
   
    // SerialUSB.print ("reading Channel ");
    // SerialUSB.print (ichan, DEC);
    // SerialUSB.print ("\n");

    uint8_t swapped_channel = arduinoAdcPinswap(ichan);

    // SerialUSB.print ("reading swapped channel ");
    // SerialUSB.print (swapped_channel, DEC);
    // SerialUSB.print ("\n");

    uint8_t analog_pin      = arduinoAnalogPin(swapped_channel);

    // SerialUSB.print ("reading analog pin ");
    // SerialUSB.print (analog_pin, DEC);
    // SerialUSB.print ("\n");

    return analogRead(analog_pin);
}

uint8_t Controller::arduinoAnalogPin (uint8_t ichan)
{
    // assert (ichan >= 0);
    // assert (ichan < NUM_ADC_CHANNELS); 

    //debug_println ("Finding arduinoAnalogPinMap");
    switch (ichan) {
        case 0x0: return PIN_A0;
        case 0x1: return PIN_A1;
        case 0x2: return PIN_A2;
        case 0x3: return PIN_A3;
        case 0x4: return PIN_A4;
        case 0x5: return PIN_A5;
        case 0x6: return PIN_A6;
        case 0x7: return PIN_A7;
        default:  return 0x0;
    }
}

uint8_t Controller::arduinoAdcPinswap (uint8_t ichan)
{
    //debug_println ("Checking arduinoAdcPinswap assertion");

    // in "Digital Logic", the order of inputs is reversed; 
    // need to unreverse it in software

    //debug_println ("arduinoAdcPinswap assertion passed");

    // assert (ichan >= 0);
    //assert (ichan < NUM_ADC_CHANNELS); 

    switch (ichan) {
        case 0x0: return 0x7;
        case 0x1: return 0x6;
        case 0x2: return 0x5;
        case 0x3: return 0x4;
        case 0x4: return 0x3;
        case 0x5: return 0x2;
        case 0x6: return 0x1;
        case 0x7: return 0x0;
        default:  return 0x0;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Switched ADC
//----------------------------------------------------------------------------------------------------------------------

uint16_t Controller::readSwitchedAdc () 
{
    return (adc.readAdc());
}

void Controller::setSwitch(uint8_t ichan)
{
    sw.setChannel(switchPinswap(ichan)); 
}

uint16_t Controller::readSwitchedAdc (uint8_t ichan) 
{
    assert (ichan >= 0); 
    assert (ichan < NUM_ADC_CHANNELS); 

    setSwitch(ichan);

    readSwitchedAdc();
}

uint16_t Controller::readSwitchedAdc (uint8_t ichan, uint8_t gain) 
{
    setSwitchGain(gain); 
    readSwitchedAdc(ichan); 
}

uint16_t Controller::autoReadSwitchedAdc (uint8_t ichan)
{
    assert (ichan >= 0); 
    assert (ichan < NUM_ADC_CHANNELS); 

    setSwitch(ichan);

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
uint8_t Controller::adcPinswap (uint8_t ichan)
{
    assert (ichan >= 0); 
    assert (ichan <  NUM_ADC_CHANNELS); 

    // Converts from channel inputs to enumerated analog inputs in the Microcontroller schematic
    // Mapping from digital_logic.SchDoc
    switch (ichan) {
        case 0x0: return 0x7;
        case 0x1: return 0x6;
        case 0x2: return 0x5;
        case 0x3: return 0x4;
        case 0x4: return 0x3;
        case 0x5: return 0x2;
        case 0x6: return 0x1;
        case 0x7: return 0x0;
        default:  return 0x0;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Multiplexer
//----------------------------------------------------------------------------------------------------------------------

void Controller::enableSwitch ()
{
    sw.enable();
}

void Controller::disableSwitch ()
{
    sw.disable();
}


void Controller::setSwitchGain (uint8_t gain)
{
    this->gain.setGain(gain);
}

uint8_t Controller::switchPinswap (uint8_t ichan)
{
    assert (ichan >= 0); 
    assert (ichan <  NUM_ADC_CHANNELS); 

    // Map pinswapping done on the ADG408 inputs which multiplex the Amplifier/ADC input
    switch (ichan) {
        case 0x0: return 0x3;
        case 0x1: return 0x2;
        case 0x2: return 0x1;
        case 0x3: return 0x0;
        case 0x4: return 0x4;
        case 0x5: return 0x5;
        case 0x6: return 0x6;
        case 0x7: return 0x7;
        default:  return 0x0;
    }
}


//----------------------------------------------------------------------------------------------------------------------
// Dac
//----------------------------------------------------------------------------------------------------------------------

void Controller::writeDac (uint8_t ichan, uint16_t value)
{
    assert (ichan >= 0); 
    assert (ichan <  NUM_ADC_CHANNELS); 
    dac.setValue(dacPinswap(ichan), value);
}

void Controller::setDacGain (uint8_t gain)
{
    dac.setGain(gain);
}

void Controller::enableDac()
{
    dac.enable();
}

void Controller::disableDac()
{
    dac.disable();
}

uint8_t Controller::dacPinswap (uint8_t ichan)
{
    assert (ichan >= 0); 
    assert (ichan <  NUM_ADC_CHANNELS); 

    switch (ichan) {
        case 0x0: return 0x3;
        case 0x1: return 0x2;
        case 0x2: return 0x1;
        case 0x3: return 0x0;
        case 0x4: return 0x5;
        case 0x5: return 0x4;
        case 0x6: return 0x7;
        case 0x7: return 0x6;
        default:  return 0x0;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// DDS
//----------------------------------------------------------------------------------------------------------------------

void Controller::setDDSFrequency (uint32_t freq)
{
    dds.setFrequency(freq); 
}

void Controller::enableCleo()
{
    cleo.enable();
}

void Controller::disableCleo()
{
    cleo.disable();
}

void Controller::selectCleo()
{
    cleo.enable();
}

void Controller::deselectCleo()
{
    cleo.disable();
}


void Controller::setGPIO (uint8_t io, bool state)
{
    assert (io>=0 && io<4);
    digitalWrite (io, state);
}

void Controller::enableGPIO (uint8_t io)
{
    pinMode(io, OUTPUT);
}
