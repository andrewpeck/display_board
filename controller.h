#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "ad567XR.h"
#include "adg408.h"
#include "ad8250.h"
#include "ltc6903.h"
#include "max1110X.h"
#include "cleo.h"

class Controller {

    public:

        Controller () :
         gain (gain_a0_pin, gain_a1_pin),
         adc  (adc_cs_pin),
         dac  (dac_gain, dac_gain_select_pin, dac_sync_pin, dac_reset_pin, dac_vref, dac_software_ldac),
         sw   (sw_en_pin, sw_a0_pin, sw_a1_pin, sw_a2_pin),
         dds  (dds_sen_pin, dds_oe_pin, dds_clk_enable, dds_clk_bar_enable),
         cleo (cleo_reset_pin, cleo_irq_pin, cleo_cs_pin)
        {
            // defaults
            sw.enable();
            enableDac();
            setDDSFrequency(1000);
        };

        uint16_t readArduinoAdc      (uint8_t ichan);

        uint16_t readSwitchedAdc     ();
        uint16_t readSwitchedAdc     (uint8_t ichan);
        uint16_t readSwitchedAdc     (uint8_t ichan, uint8_t gain);

        uint16_t autoReadSwitchedAdc (uint8_t ichan);
        void     setSwitchGain       (uint8_t gain);
        void     setSwitch           (uint8_t ichan);
        void     enableSwitch();
        void     disableSwitch();


        void     setGPIO             (uint8_t io, bool state);
        void     enableGPIO          (uint8_t io);

        void     writeDac            (uint8_t ichan, uint16_t value);

        void      enableDac();
        void     disableDac();
        void     setDacGain(uint8_t gain);

        void      enableCleo();
        void     disableCleo();

        void       selectCleo();
        void     deselectCleo();

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
        enum {pa23=0, pa22=1, pa21=2, pa20=3, pa13=4, pa12=5, pa15=6, pb22=7, pb10=8, pb11=9, pb23=10, pa14=11, pb2=12, pa28=13, pa8=14, pa9=15, pa10=16, pa11=17, pa18=28};

        // map taken from display board schematic; convert from io# to PA/BX
        enum {io4=pb23, io9=pb10, io10=pb11, io6=pa13, io5=pa12, io13=pa14, io1=pa20, io2=pa21, io7=pa22, io8=pa23, io14=pa15, io11=pb22, spi0_cs=pa18, analog9=pb2, io3=pa28};

        const uint8_t gpio [4] = { pa8, pa9, pa10, pa11}

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

        uint8_t dac_gain = 2;

        const uint8_t dac_gain_select_pin = io14;
        const uint8_t dac_sync_pin        = spi0_cs;
        const uint8_t dac_reset_pin       = io11;

        float   dac_vref          = 4.096f;
        bool    dac_software_ldac = false;

        //--------------------------------------------------------------------------------------------------------------
        // Cleo
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t cleo_reset_pin = io4;
        const uint8_t cleo_irq_pin   = io3;
        const uint8_t cleo_cs_pin    = analog9;

        //--------------------------------------------------------------------------------------------------------------
        // DDS
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t dds_sen_pin        = io9 ;
        const uint8_t dds_oe_pin         = io10;
        const uint8_t dds_clk_enable     = 1;
        const uint8_t dds_clk_bar_enable = 0;

        //--------------------------------------------------------------------------------------------------------------
        // Members Only
        //--------------------------------------------------------------------------------------------------------------

        AD8250   gain ;
        Max11101 adc  ;
        AD5672R  dac  ;
        ADG408   sw   ;
        LTC6903  dds  ;
        CleOCtrl cleo ;

};

#endif /* CONTROLLER_H */
