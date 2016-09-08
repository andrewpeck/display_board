#ifndef MAX11101_H
#define MAX11101_H

#include <stdint.h>

class Max11101 : public Max1110X {
    public: 
        Max11101 (uint8_t cs_pin) : Max1110X (cs_pin, 12) {}  // 12 bit version
};

class Max11100 : public Max1110X {
    public: 
        Max11100 (uint8_t cs_pin) : Max1110X (cs_pin, 16) {}  // 16 bit version
};

class Max1110X {
    public:

        Max1110X (uint8_t cs_pin, uint8_t resolution);

        float    readVoltage();
        uint16_t readADC    ();
        uint16_t maxCounts  ();
        float    maxVoltage ();

    private:
        struct Config {
            uint8_t cs_pin; 
            float   vref=4.096f; 
            uint8_t resolution; 
        } config;
} ;

#endif /* MAX11101_H */
