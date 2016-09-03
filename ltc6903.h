#ifndef LTC6903_H
#define LTC6903_H

#include <stdint.h>

class LTC6903
{
    public: 

        LTC6903 (uint8_t sen_pin, uint8_t oe_pin, bool clk_enable, bool clk_bar_enable);

        void setFrequency(uint32_t frequency);
        void enable();
        void disable();

    private:

        void setFrequency(uint8_t oct, uint8_t dac);
        uint8_t generateDac (uint32_t frequency, uint8_t oct);
        uint8_t generateOct (uint32_t frequency);

        uint8_t generateCnf (bool clk_enable, bool clk_bar_enable);

        uint16_t buildWord (uint8_t oct, uint8_t dac);

        struct Config {
            uint8_t sen_pin;
            uint8_t oe_pin;
            bool    clk_bar_enable;
            bool    clk_enable;
            uint8_t cnf;
        } config;

};

#endif /* LTC6903_H */
