#include "controller.h"
#include "high_voltage.h"
#include "screen.h"

#include <SPI.h>

Controller controller;


float max_voltage_scaler = 1.1;
float min_voltage_scaler = 0.9;
float max_current_scaler = 1.1;


#define Serial SerialUSB

int last_time =0;

int counter    = 0;
int dac_counts = 0;

bool ovp_ok        = 1;
bool ovc_ok        = 1;
bool status_ok     = 1;

bool     output_on [2] = {0,0};
uint16_t max_voltage = 0;
uint16_t max_current = 0;

uint16_t voltage_max [2] = {0,0}; // holds the current maximum allowed voltage (in volts)
uint16_t current_max [2] = {0,0};
uint16_t voltage_min [2] = {0,0};

uint16_t set_voltage [2]      = {0,0};  // Set Voltage IN Volts
uint16_t set_current [2]      = {0,0};  // Set Current IN Centi-Amps (10s of mA -- needed for 1 decimal place)

uint16_t read_voltage_counts [2]      = {0,0};  // Current reading in counts
uint16_t read_current_counts [2]      = {0,0};  // Current reading in counts

//-Voltage + Current Reading Smoothing----------------------------------------------------------------------------------
const int num_boxcars = 100;
uint8_t   read_index  = 0;
uint16_t read_voltage_counts_arr [2][num_boxcars];
uint16_t read_current_counts_arr [2][num_boxcars];
uint32_t read_voltage_counts_total [2];
uint16_t read_voltage_counts_average[2];
uint32_t read_current_counts_total [2];
uint16_t read_current_counts_average[2];

uint16_t last_set_voltage [2] = {0,0};
uint16_t last_set_current [2] = {0,0};


uint16_t adc_fast_reading [8];

void setup () {

    //------------------------------------------------------------------------------------------------------------------
    // Start Serial
    //------------------------------------------------------------------------------------------------------------------

    SerialUSB.begin(0);

    //------------------------------------------------------------------------------------------------------------------
    // Start SPI
    //------------------------------------------------------------------------------------------------------------------
    SerialUSB.println("UCLA Analog Display Board");

    SerialUSB.println("Starting SPI...");

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setBitOrder(MSBFIRST);

    SerialUSB.println("Started SPI...");

    SerialUSB.println("Setting DDS Frequency");

    //------------------------------------------------------------------------------------------------------------------
    // Reset Averaging
    //------------------------------------------------------------------------------------------------------------------

    for (int i=0; i<num_boxcars; i++) {
        read_voltage_counts_arr [0][i] = 0;
        read_voltage_counts_arr [1][i] = 0;
        read_current_counts_arr [0][i] = 0;
        read_current_counts_arr [1][i] = 0;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Configure Arduino
    //------------------------------------------------------------------------------------------------------------------

    analogReadResolution(16);

    pinMode(12, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(13, INPUT);

    digitalWrite(10, LOW);
    digitalWrite(10, HIGH);


    //------------------------------------------------------------------------------------------------------------------
    // Write DAC
    //------------------------------------------------------------------------------------------------------------------

    controller.enableDac();
    controller.enableSwitch();
    controller.setDacGain(2);

    for (int ichan=0; ichan<8; ichan++) {
        controller.writeDac(ichan, output_zerovolt_safe);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Power Supply Enables
    //------------------------------------------------------------------------------------------------------------------
    setEnable(0, 0);
    setEnable(0, 1);
    configureEnables();


    //------------------------------------------------------------------------------------------------------------------
    // CleO
    //------------------------------------------------------------------------------------------------------------------

    SerialUSB.println("Starting Cleo");
    /* Initialize CleO - needs to be done only once */

    CleO.begin();
    CleO.Start();
    CleO.RectangleJustification(MM);
    CleO.SetBackgroundcolor(background_color);
    CleO.Show();

    CleO.DisplayRotate(2, 0);
    font = CleO.LoadFont("@Fonts/DSEG7ClassicMini-BoldItalic.ftfont");
}


void loop () {

    //------------------------------------------------------------------------------------------------------------------
    // Diagnostic Printouts
    //------------------------------------------------------------------------------------------------------------------

    if (1==0) printCurrentTag();

    if (counter==0) {SerialUSB.println("starting loop");};

    if ((counter%1000)==0) {
        SerialUSB.print("milliseconds per loop: ");
        SerialUSB.print((millis()-last_time) / 1000);
        SerialUSB.print("\n");
        last_time = millis();
    }

    // Read ADCs with Boxcar Smoothing (sample count controlled by num_boxcars)
    //------------------------------------------------------------------------------------------------------------------
    fastReadAdcs();

    // Check if voltages and currents are within tolerance
    //------------------------------------------------------------------------------------------------------------------
    checkMinMax();

    // Update DAC
    //------------------------------------------------------------------------------------------------------------------
    if (counter%1==0) { // want to read ADCs fast.. but can think about skipping DAC writes
        updateDacs();
    }

    // Refresh Screen -- skip loops to avoid unnecessarily high refresh rate
    //------------------------------------------------------------------------------------------------------------------
    if (counter%5==0) {
        updateScreen();
    }

    // Update Min/Max voltage parameters (based on possible new values from screen)
    //------------------------------------------------------------------------------------------------------------------
    updateMinMax();

    for (int i=0; i<2; i++) {
        if (ovc[i]) shutdown[i]
    }

    // Increment Loop Counter
    //------------------------------------------------------------------------------------------------------------------
    counter++;
}

void checkMinMax()
{
    // use the NON-averaged values here.. don't want to be too slow. But will this be too susceptible to transients?
    for (int panel=0; panel<2; panel++) {

        //-max voltage------------------------------------------------------------------------------------------------------
        if (read_voltage_counts[panel] > voltage_max[panel])
            ovp_ok = 0;
        //-min voltage------------------------------------------------------------------------------------------------------
        if (read_voltage_counts[panel] < voltage_min[panel])
            ovp_ok = 0;
        else
            ovp_ok = 1;


        //-max current------------------------------------------------------------------------------------------------------
        ovc_ok = (read_current_counts[panel] < current_max[panel]) ? 1 : 0;

    }
}

void updateMinMax()
{
    for (int panel=0; panel<2; panel++) {
        voltage_max[panel] = set_voltage[panel] * max_voltage_scaler;

        current_max[panel] = set_current[panel] * max_current_scaler;

        voltage_min[panel] = set_voltage[panel] * min_voltage_scaler;
    }

}

void updateDacs()
{
    for (int ichan=0; ichan<2; ichan++) {
        if (!output_on[ichan]) { // turn off the output if the output is off. duh!
            controller.writeDac(ichan, output_zerovolt_safe);
            last_set_voltage[ichan] = 0; // make sure this gets set here, or we won't write the dac when enabling the output
        }
        else if (set_voltage[ichan]!=last_set_voltage[ichan]) { // no point in re-writing the same voltage on the dac

            uint16_t counts = voltageToDacCounts(set_voltage[ichan]);
            uint16_t inverted_counts = invertCounts (counts);

            last_set_voltage[ichan]=set_voltage[ichan];

            SerialUSB.print("setting dac ");
            SerialUSB.print(ichan);
            SerialUSB.print(" to ");
            SerialUSB.print(set_voltage[ichan]);
            SerialUSB.print(" = (");
            SerialUSB.print(inverted_counts);
            SerialUSB.print(" inverted dac_counts) ");
            SerialUSB.print(" , (");
            SerialUSB.print(counts);
            SerialUSB.print(" non-inverted dac_counts)");
            SerialUSB.print("\n");

            controller.writeDac(ichan, inverted_counts);
        }
    }
}

void fastReadAdcs() {
    for (int i=0; i<8; i++) {
        adc_fast_reading[i] = controller.readArduinoAdc(i);
    }

    for (int i=0; i<2; i++) {
        read_voltage_counts[i] = adc_fast_reading[i];
        read_current_counts[i] = adc_fast_reading[i+2];

        read_voltage_counts_total[i] =read_voltage_counts_total[i] - read_voltage_counts_arr [i][read_index] + read_voltage_counts[i];
        read_current_counts_total[i] =read_current_counts_total[i] - read_current_counts_arr [i][read_index] + read_current_counts[i];

        read_voltage_counts_arr[i][read_index] = read_voltage_counts[i];
        read_current_counts_arr[i][read_index] = read_current_counts[i];

        read_voltage_counts_average[i] = read_voltage_counts_total[i]/num_boxcars;
        read_current_counts_average[i] = read_current_counts_total[i]/num_boxcars;

    }

    read_index++;
    if (read_index >= num_boxcars) {
        read_index = 0;
    }
}
