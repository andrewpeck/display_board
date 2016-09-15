#ifndef MAX1110X_H
#define MAX1110X_H

#include <stdint.h>

class Max1110X {
    public:

        float    readVoltage();
        uint16_t readAdc    ();
        uint16_t maxCounts  ();
        float    maxVoltage ();

    protected:

        void     initialize(uint8_t cs_pin, uint8_t resolution);

    private:
        void select();
        void deselect();
        uint16_t bitmask();

        struct Config {
            uint8_t cs_pin; 
            float   vref=4.096f; 
            uint8_t resolution; 
        } config;
} ;

class Max11101 : public Max1110X {
    public: 
        Max11101 (uint8_t cs_pin) : Max1110X () {initialize(cs_pin, 12);}  // 12 bit version
};

class Max11100 : public Max1110X {
    public: 
        Max11100 (uint8_t cs_pin) : Max1110X () {initialize(cs_pin, 12);}  // 16 bit version
};

//----------------------------------------------------------------------------------------------------------------------
#endif /* MAX1110X_H */
//----------------------------------------------------------------------------------------------------------------------
