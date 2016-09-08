#ifndef AD567XR_H
#define AD567XR_H

#include <stdint.h>

class AD5672R : public AD567XR {
    public: 
        AD5672R (
                uint8_t gain            = 1,
                uint8_t gain_select_pin = -1,
                uint8_t sync_pin        = -1,
                uint8_t reset_pin       = -1,
                float   vref            = 4.096f,
                bool    software_ldac   = false
                ) : AD567XR (gain, gain_select_pin, sync_pin, reset_pin, vref, software_ldac, 12) {}  // 12 bit version
}; 

class AD5676R : public AD567XR {
    public: 
        AD5676R (
                uint8_t gain            = 1,
                uint8_t gain_select_pin = -1,
                uint8_t sync_pin        = -1,
                uint8_t reset_pin       = -1,
                float   vref            = 4.096f,
                bool    software_ldac   = false
                ) : AD567XR (gain, gain_select_pin, sync_pin, reset_pin, vref, software_ldac, 16) {}  // 16 bit version
}; 

class AD567XR {

    public:

        //--------------------------------------------------------------------------------------------------------------
        // Constructor
        //--------------------------------------------------------------------------------------------------------------

        AD567XR (
                uint8_t gain            = 1,
                uint8_t gain_select_pin = -1,
                uint8_t sync_pin        = -1,
                uint8_t reset_pin       = -1,
                float   vref            = 4.096f,
                bool    software_ldac   = false
                uint8_t resolution      = 12,
                );

        //--------------------------------------------------------------------------------------------------------------
        // Public Functions
        //--------------------------------------------------------------------------------------------------------------

        void     setValue         (uint8_t channel, uint16_t value);
        void     setVoltage       (uint8_t channel, float    voltage);
        void     setResolution    (uint8_t resolution); 

    private:

        //--------------------------------------------------------------------------------------------------------------
        // Private Functions
        //--------------------------------------------------------------------------------------------------------------

        uint32_t buildPacket   (uint8_t command, uint8_t address, uint16_t data);

        uint16_t maxDacCounts ();

        //--------------------------------------------------------------------------------------------------------------
        // Configuration
        //--------------------------------------------------------------------------------------------------------------

        struct Config {
            uint8_t gain=1;
            uint8_t resolution;

            uint8_t gain_select_pin;
            uint8_t sync_pin;
            uint8_t reset_pin;

            float   vref;
            bool    software_ldac;
        } config;

        //--------------------------------------------------------------------------------------------------------------
        // DAC Commands
        //--------------------------------------------------------------------------------------------------------------

        static const uint8_t CMD_NOOP               = 0x0; // 0000
        static const uint8_t CMD_WRITE_DAC          = 0x1; // 0001
        static const uint8_t CMD_LOAD_DAC           = 0x2; // 0010
        static const uint8_t CMD_WRITE_AND_LOAD_DAC = 0x3; // 0011
        static const uint8_t CMD_POWER_DOWN         = 0x4; // 0100
        static const uint8_t CMD_POWER_UP           = 0x5; // 0101
        static const uint8_t CMD_SOFT_LDAC          = 0x6; // 0110
        static const uint8_t CMD_INTREF_GAIN        = 0x7; // 0111
        static const uint8_t CMD_DAISYCHAIN         = 0x8; // 1000
        static const uint8_t CMD_READBACK           = 0x9; // 1001
        static const uint8_t CMD_UPDATE_ALL         = 0xa; // 1010
        static const uint8_t CMD_WIRE_AND_LOAD_ALL  = 0xb; // 1011
        static const uint8_t CMD_RESERVED1          = 0xc; // 1100
        static const uint8_t CMD_RESERVED2          = 0xd; // 1101
        static const uint8_t CMD_RESERVED3          = 0xe; // 1110
        static const uint8_t CMD_RESERVED4          = 0xf; // 1111
};


#endif /* AD567XR_H */
