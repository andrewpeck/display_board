#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "ad5672R.h"
#include "adg408.h"
#include "ad8250.h"
#include "ltc6903.h"
#include "max11101.h"

class Controller {
    public:
        Controller () :
         gain (gain_a0_pin, gain_a1_pin),
         adc  (adc_cs_pin),
         dac  (dac_gain, dac_resolution, dac_gain_select_pin, dac_sync_pin, dac_reset_pin, dac_vref, dac_software_ldac),
         sw   (sw_en_pin, sw_a0_pin, sw_a1_pin, sw_a2_pin),
         dds  (dds_sen_pin, dds_oe_pin, dds_clk_enable, dds_clk_bar_enable)
        {};

        uint16_t readAdc         (uint8_t ichan);
        uint16_t readSwitchedAdc (uint8_t ichan);
        void     writeDac        (uint8_t ichan);
        void     setSwitchGain   (uint8_t gain);
        void     setDDSFrequency (uint32_t freq);

    private:

        static const int NUM_ADC_CHANNELS=8;

        uint8_t adcPinswap    (uint8_t ichan);
        uint8_t adcArduinoMap (uint8_t ichan);
        uint8_t dacPinswap    (uint8_t ichan);
        uint8_t switchPinswap (uint8_t ichan);
        uint8_t obufPinswap   (uint8_t ichan);

        //--------------------------------------------------------------------------------------------------------------
        // IO Map
        //--------------------------------------------------------------------------------------------------------------

        // map taken from display board schematic; convert from io# into Pin Package Number
        enum {io9=19, io10=20, io6=22, io5=21, io13=23, io1=29, io2=30, io7=31, io8=32, io14=24, io11=37, pa18=27};

        uint8_t pin2arduino (uint8_t io)
        {
            // look at "Digital Logic" section of schematics... use samd21.SchDoc port names and convert to 
            // convert from digital logic Samd21 IO names; these are enummed to Package Pin numbers. 
            // map the package pin numbers to Arduino digitalWrite numbers
            switch (io) {
                case io9  : return 0;  // pin 19 = pb10_s4_spi_mosi
                case io10 : return 0;  // pin 20 = pb11_s4_spi_sck
                case io6  : return 0;  // pin 22 = pa13_gpio
                case io5  : return 0;  // pin 21 = pb12_s4_spi_miso
                case io13 : return 0;  // pin 23 = pa08_tcc0-w0
                case io1  : return 0;  // pin 29 = pa20_tcc0-w6
                case io2  : return 0;  // pin 30 = dgi_gpio1
                case io7  : return 0;  // pin 31 = sda
                case io8  : return 0;  // pin 32 = scl
                case io14 : return 0;  // pin 24 = pa15_tcc0-w5
                case io11 : return 0;  // pin 37 = txd/3.1b
                case pa18 : return 0;  // pin 27 = ss/3.7c
                default   : return 0;
            }
        }

        //--------------------------------------------------------------------------------------------------------------
        // DDS
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t dds_sen_pin        = pin2arduino(io9 );
        const uint8_t dds_oe_pin         = pin2arduino(io10);
        const uint8_t dds_clk_enable     = 1;
        const uint8_t dds_clk_bar_enable = 0;

        //--------------------------------------------------------------------------------------------------------------
        // Gain
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t gain_a0_pin = pin2arduino(io6);
        const uint8_t gain_a1_pin = pin2arduino(io5);

        //--------------------------------------------------------------------------------------------------------------
        // ADC
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t adc_cs_pin = pin2arduino(io13);

        //--------------------------------------------------------------------------------------------------------------
        // Switch Pins
        //--------------------------------------------------------------------------------------------------------------

        const uint8_t sw_en_pin = pin2arduino(io1);
        const uint8_t sw_a0_pin = pin2arduino(io2);
        const uint8_t sw_a1_pin = pin2arduino(io7);
        const uint8_t sw_a2_pin = pin2arduino(io8);

        //--------------------------------------------------------------------------------------------------------------
        // Dac
        //--------------------------------------------------------------------------------------------------------------
        uint8_t dac_gain       = 2;
        uint8_t dac_resolution = 12;

        const uint8_t dac_gain_select_pin = pin2arduino(io14);
        const uint8_t dac_sync_pin        = pin2arduino(pa18);
        const uint8_t dac_reset_pin       = pin2arduino(io11);

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
