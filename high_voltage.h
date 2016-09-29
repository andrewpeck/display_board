#ifndef HIGH_VOLTAGE_H
#define HIGH_VOLTAGE_H

//----------------------------------------------------------------------------------------------------------------------
// Controller parameters
//----------------------------------------------------------------------------------------------------------------------

int      dac_vref = 5; // TODO: this should be changed to 2.5 * dac_gain
uint32_t dac_full_scale = (1<<16)-1;

const uint32_t input_divider  = 2; // Voltage divider at the input of the control board

const float      input_scaler  [8] = {1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0}; // scale the gain of the inputs to correct error in the voltage divider
const uint32_t   output_offset [8] = {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0};   // output offset voltage

const uint32_t adc_vref = 33; // uC adc voltage reference in deciVolts == 3.3V

//----------------------------------------------------------------------------------------------------------------------
// Power Supply Parameters
//----------------------------------------------------------------------------------------------------------------------

const uint32_t hv_divider     = 820; // V/V -- X volts high-voltage output / Y volts monitor output
const uint32_t hv_multiplier  = 820; // V/V -- X volts high-voltage output / Y volts dac input
const uint32_t current_scaler = 5; // V/A (Ohms!) -- X volts current monitor output / Y Amps high-voltage current

const float    output_zerovolt_voltage = 3.2f; // magic number 3.2V input = 0 V output
const uint32_t output_zerovolt_counts  = ((output_zerovolt_voltage * (dac_full_scale))/dac_vref); // output Zero in DAC counts
const uint32_t output_zerovolt_safe    = output_zerovolt_counts + 1000; // offset this so we aren't on the cusp;; make sure we are zeroed

const uint16_t imposed_voltage_cap = 2500;
const uint16_t natural_voltage_cap = output_zerovolt_voltage * hv_multiplier;
const uint16_t abs_max_voltage     = (imposed_voltage_cap < natural_voltage_cap) ? imposed_voltage_cap : natural_voltage_cap; // use the lowest voltage cap
const uint16_t abs_max_current     = 100; // 10 * mA

const uint32_t V_per_uV = 10;

float max_voltage_scaler = 1.1;
float min_voltage_scaler = 0.9;
float max_current_scaler = 1.1;

//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------

uint32_t microAmpsToCounts()
{
}

// convert a number of dac counts into micro-amps (for storing as an integer)
uint32_t countsToMicroAmps(uint16_t counts)
{
    uint32_t microamps = (1000/10) * uint32_t (adc_vref * current_scaler * counts * input_divider) / (dac_full_scale);
    // divide by 10 to compensate for adc_vref

//    SerialUSB.print ("counts: ");
//    SerialUSB.print (counts);
//    SerialUSB.print (" ;     ");
//
//    SerialUSB.print ("microamps: ");
//    SerialUSB.print (microamps);
//    SerialUSB.print ("\n");
    return (
            microamps
            );
}

// convert a number of dac counts into decivolts
uint16_t countsToDeciVolts(uint16_t counts)
{
   uint16_t deciVolts = (1) * uint32_t (adc_vref * hv_divider * counts * input_divider) / (dac_full_scale);

   // SerialUSB.print ("counts: ");
   // SerialUSB.print (counts);
   // SerialUSB.print ("\n");
   // SerialUSB.print ("deciVolts: ");
   // SerialUSB.print (deciVolts);
   // SerialUSB.print ("\n");
   // SerialUSB.print (adc_vref);
   // SerialUSB.print (", ");
   // SerialUSB.print (hv_divider);
   // SerialUSB.print (", ");
   // SerialUSB.print (counts);
   // SerialUSB.print (", ");
   // SerialUSB.print (input_divider);
   // SerialUSB.print (", ");
   // SerialUSB.print (V_per_uV);
   // SerialUSB.print (", ");
   // SerialUSB.print (dac_full_scale);
   // SerialUSB.print ("\n");

   return (deciVolts);
}

// convert an output voltage (in Volts) to a # of DAC counts (non-inverted, proportional)
uint16_t voltsToDacCounts (uint16_t voltage)
{
    return ( voltage * dac_full_scale / (dac_vref * hv_multiplier));
}

// convert an output voltage (in Volts) to a # of DAC counts (non-inverted, proportional)
uint16_t deciVoltsToDacCounts (uint16_t voltage)
{
    return (voltage * dac_full_scale / ( 10 * dac_vref * hv_multiplier));
}

// Control of the power supply is INVERTED... need to reduce set voltage to increase the output voltage
uint16_t invertCounts (uint16_t counts)
{
    return ((output_zerovolt_counts - counts));
}

void enableChannel (uint8_t channel)
{
    controller.setGPIO(channel, 0);
}

void disableChannel (uint8_t channel)
{
    controller.setGPIO(channel, 1);
}

void configureEnables ()
{
    controller.enableGPIO(0);
    controller.enableGPIO(1);
}

#endif /* HIGH_VOLTAGE_H */
