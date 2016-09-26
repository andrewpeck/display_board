#ifndef HIGH_VOLTAGE_H
#define HIGH_VOLTAGE_H 

int dac_vref = 5;
uint32_t dac_full_scale = (1<<16)-1;
//----------------------------------------------------------------------------------------------------------------------
// Power Supply Parameters
//----------------------------------------------------------------------------------------------------------------------
const uint32_t hv_divider     = 820; // V/V -- X volts high-voltage output / Y volts monitor output
const uint32_t hv_multiplier  = 820; // V/V -- X volts high-voltage output / Y volts dac input
const uint32_t current_scaler = 5; // V/A (Ohms!) -- X volts current monitor output / Y Amps high-voltage current
const uint32_t input_divider  = 2; // Voltage divider at the input of the control board

const float input_scaler [8]  = {1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0 , 1.0}; // scale the gain of the inputs to correct error in the voltage divider
const uint32_t   output_offset [8] = {0   , 0   , 0   , 0   , 0   , 0   , 0   , 0}; // output offset voltage

const float    output_zerovolt_voltage = 2.5f;
const uint32_t output_zerovolt         = ((output_zerovolt_voltage * (dac_full_scale))/5.0);  // output Zero in DAC counts
const uint32_t output_zerovolt_safe    = output_zerovolt + 1000;

const uint16_t abs_max_voltage = output_zerovolt_voltage * hv_multiplier;
const uint16_t abs_max_current = 100;

const uint32_t adc_vref = 33; // uC adc voltage reference in deciVolts

const uint32_t V_per_uV         = 10;

uint16_t centiAmpsToCounts()
{
}

// convert a number of dac counts into centi-amps (for storing as an integer)
uint16_t countsToCentiAmps(uint16_t counts)
{
    return ((10) * uint32_t (adc_vref * current_scaler * counts * input_divider) / (dac_full_scale) );
}

// convert a number of dac counts into decivolts
uint16_t countsToDeciVolts(uint16_t counts)
{
   uint16_t deciVolts = (1) * uint32_t (adc_vref * hv_divider * counts * input_divider) / (dac_full_scale);

   SerialUSB.print ("counts: ");
   SerialUSB.print (counts);
   SerialUSB.print ("\n");
   SerialUSB.print ("deciVolts: ");
   SerialUSB.print (deciVolts);
   SerialUSB.print ("\n");
   SerialUSB.print (adc_vref);
   SerialUSB.print (", ");
   SerialUSB.print (hv_divider);
   SerialUSB.print (", ");
   SerialUSB.print (counts);
   SerialUSB.print (", ");
   SerialUSB.print (input_divider);
   SerialUSB.print (", ");
   SerialUSB.print (V_per_uV);
   SerialUSB.print (", ");
   SerialUSB.print (dac_full_scale);
   SerialUSB.print ("\n");
   return (deciVolts);
}

uint16_t deciVoltstoCounts()
{
}

// convert an output voltage (in Volts) to a # of DAC counts (non-inverted, proportional)
uint16_t voltageToDacCounts (uint16_t voltage)
{
    return ( voltage * dac_full_scale / (dac_vref * hv_multiplier));
}

// Control of the power supply is INVERTED... need to reduce set voltage to increase the output voltage
uint16_t invertCounts (uint16_t counts)
{
    return ((output_zerovolt - counts));
}

#endif /* HIGH_VOLTAGE_H */
