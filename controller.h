#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "ad567XR.h"
#include "adg408.h"
#include "ad8250.h"
#include "ltc6903.h"
#include "max1110X.h"

class Controller {

    public:

        Controller () :
         gain (gain_a0_pin, gain_a1_pin),
         adc  (adc_cs_pin),
         dac  (dac_gain, dac_gain_select_pin, dac_sync_pin, dac_reset_pin, dac_vref, dac_software_ldac),
         sw   (sw_en_pin, sw_a0_pin, sw_a1_pin, sw_a2_pin),
         dds  (dds_sen_pin, dds_oe_pin, dds_clk_enable, dds_clk_bar_enable)
        {};

        uint16_t readArduinoAdc      (uint8_t ichan);

        uint16_t readSwitchedAdc     ();
        uint16_t readSwitchedAdc     (uint8_t ichan);
        uint16_t readSwitchedAdc     (uint8_t ichan, uint8_t gain);

        uint16_t autoReadSwitchedAdc (uint8_t ichan);
        void     setSwitchGain       (uint8_t gain);

        void     writeDac            (uint8_t ichan, uint16_t value);
        void     setDDSFrequency     (uint32_t freq);

    private:

        static const int NUM_ADC_CHANNELS=8;

        uint8_t adcPinswap       (uint8_t ichan);
        uint8_t switchPinswap    (uint8_t ichan);

        uint8_t dacPinswap       (uint8_t ichan);
        uint8_t obufPinswap      (uint8_t ichan);

        uint8_t arduinoAdcPinswap (uint8_t ichan);
        uint8_t arduinoAnalogPin  (uint8_t ichan);

        //--------------------------------------------------------------------------------------------------------------
        // IO Map
        //--------------------------------------------------------------------------------------------------------------

        // map taken from variants.cpp
        enum {pa23=0, pa22, pa21, pa20, pa13, pa12, pa15, pb22, pb10, pb11, pb23, pa14, pb2, pa28, pa8, pa9, pa10, pa11};

        // map taken from display board schematic; convert from io# to PA/BX
        enum {io9=pb10, io10=pb11, io6=pa13, io5=pa12, io13=pa8, io1=pa20, io2=pa21, io7=pa22, io8=pa23, io14=pa15, io11=pb22, pa18=27};

        //--------------------------------------------------------------------------------------------------------------
        // DDS
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t dds_sen_pin        = io9 ;
        const uint8_t dds_oe_pin         = io10;
        const uint8_t dds_clk_enable     = 1;
        const uint8_t dds_clk_bar_enable = 0;

        //--------------------------------------------------------------------------------------------------------------
        // Gain
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t gain_a0_pin = io6;
        const uint8_t gain_a1_pin = io5;

        //--------------------------------------------------------------------------------------------------------------
        // ADC
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t adc_cs_pin = io13;

        //--------------------------------------------------------------------------------------------------------------
        // Switch Pins
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t sw_en_pin = io1;
        const uint8_t sw_a0_pin = io2;
        const uint8_t sw_a1_pin = io7;
        const uint8_t sw_a2_pin = io8;

        //--------------------------------------------------------------------------------------------------------------
        // Dac
        //--------------------------------------------------------------------------------------------------------------

        uint8_t dac_gain       = 2;

        const uint8_t dac_gain_select_pin = io14;
        const uint8_t dac_sync_pin        = pa18;
        const uint8_t dac_reset_pin       = io11;

        float   dac_vref          = 4.096f;
        bool    dac_software_ldac = false;

        //--------------------------------------------------------------------------------------------------------------
        // Members Only
        //--------------------------------------------------------------------------------------------------------------

        AD8250   gain ;
        Max11101 adc  ;
        AD5672R  dac  ;
        ADG408   sw   ;
        LTC6903  dds  ;

};

#endif /* CONTROLLER_H */
