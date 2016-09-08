#include "controller.h"
#include "SPI.h"

Controller controller;

void setup () {

    SerialUSB.begin(0);
    SerialUSB.println("UCLA Analog Display Board");

    SerialUSB.println("Starting SPI...");

    SPI.begin();
}

void loop () {

    for (int ichan=0; ichan<8; ichan++) {
        uint8_t value = controller.readArduinoAdc(ichan);
        controller.writeDac(ichan, value);
    }

}

