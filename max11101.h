#ifndef MAX11101_H
#define MAX11101_H

#include <stdint.h>

class Max11101 {
    public:

        Max11101 (uint8_t cs_pin);

        float    readVoltage();
        uint16_t readADC    ();
        uint16_t maxCounts  ();
        float    maxVoltage ();

    private:
        struct Config {
            uint8_t cs_pin; 
            float   vref=4.096f; 
        } config;
} ;

#endif /* MAX11101_H */
