#ifndef CLEO_H
#define CLEO_H 

#include <stdint.h>

class CleOCtrl {
    public:
        CleOCtrl (uint8_t reset_pin, uint8_t irq_pin, uint8_t cs_pin);

        void enable();
        void disable();
        void select();
        void deselect();
    private:
        struct Config {
            uint8_t reset_pin;
            uint8_t irq_pin;
            uint8_t cs_pin;
        } config;
};
#endif /* CLEO_H */
