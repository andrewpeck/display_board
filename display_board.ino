#include "controller.h"
Controller controller;

#include <SPI.h>
#define Serial SerialUSB

#include "high_voltage.h"
#include "screen.h"

int last_time =0;

int counter    = 0;

//----------------------------------------------------------------------------------------------------------------------
// Status and Readings
//----------------------------------------------------------------------------------------------------------------------

bool     ovp_ok    [2]           = {1,1};
bool     ovc_ok    [2]           = {1,1};
bool     status_ok [2]           = {1,1};

bool     output_on [2]           = {0,0};

uint16_t decivolts_max [2]       = {0,0}; // holds the current allowed voltage (in deciVolts)
uint32_t microamps_max [2]       = {0,0}; // holds the maximum allowed current (in microAmps)
uint16_t decivolts_min [2]       = {0,0};

uint16_t set_decivolts [2]       = {0,0};  // Set Voltage IN deciVolts
uint32_t set_microamps [2]       = {0,0};  // Set Current IN microAmps (10s of mA -- needed for 1 decimal place)

uint16_t read_voltage_counts [2] = {0,0};  // Current reading in counts
uint16_t read_current_counts [2] = {0,0};  // Current reading in counts

uint16_t read_decivolts_fast [2] = {0,0};  // Current reading in decivolts
uint32_t read_microamps_fast [2] = {0,0};  // Current reading in microamps

uint16_t last_set_decivolts [2]  = {0,0};
uint32_t last_set_microamps [2]  = {0,0};

uint16_t adc_fast_reading [8]    = {0, 0, 0, 0, 0, 0, 0, 0}; // 8 adc channels


//----------------------------------------------------------------------------------------------------------------------
// Smoothing
//----------------------------------------------------------------------------------------------------------------------

const int num_boxcars = 128; // should use a power of 2 here if you can help it..
uint8_t   read_index  = 0;

uint16_t read_voltage_counts_train [2][num_boxcars];
uint16_t read_current_counts_train [2][num_boxcars];

uint16_t read_voltage_counts_average[2];
uint16_t read_current_counts_average[2];

uint32_t read_voltage_counts_total [2];
uint32_t read_current_counts_total [2];

//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------

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
        read_voltage_counts_train [0][i] = 0;
        read_voltage_counts_train [1][i] = 0;
        read_current_counts_train [0][i] = 0;
        read_current_counts_train [1][i] = 0;
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
    disableChannel(0);
    disableChannel(1);
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


bool state = 0;
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

        state = !state;
    }

    // Read ADCs with Boxcar Smoothing (sample count controlled by num_boxcars)
    //------------------------------------------------------------------------------------------------------------------
    if (counter%1==0) {
    fastReadAdcs();
    }

    // Check if voltages and currents are within tolerance;
    // Shutdown outputs if over
    //------------------------------------------------------------------------------------------------------------------
    if (counter%1==0) {
    checkMinMax();
    }

    for (int i=0; i<2; i++) {
    if (!ovc_ok[i]) disableChannel(i);
    }


    // Update DAC
    //------------------------------------------------------------------------------------------------------------------
    if (counter%5==0) { // want to read ADCs fast.. but can think about skipping DAC writes
        updateDacs();
    }

    // Refresh Screen -- skip loops to avoid unnecessarily high refresh rate
    //------------------------------------------------------------------------------------------------------------------
    if (counter%3==0) {
        updateScreen();
    }

    // Update Min/Max voltage parameters (based on possible new values from screen)
    //------------------------------------------------------------------------------------------------------------------
    if (counter%1==0) {
    updateMinMax();
    }

    // Check "Steady State"
    //------------------------------------------------------------------------------------------------------------------

    if (counter%10==0) {
        checkStability();
    }

    // Increment Loop Counter
    //------------------------------------------------------------------------------------------------------------------
    counter++;
}

void checkMinMax()
{
    // use the NON-averaged values here.. don't want to be too slow. But will this be too susceptible to transients?
    for (int panel=0; panel<2; panel++) {

        //-max voltage--------------------------------------------------------------------------------------------------
        if (read_decivolts_fast[panel] > decivolts_max[panel])
            ovp_ok[panel] = 0;
        //-min voltage--------------------------------------------------------------------------------------------------
        else if (read_decivolts_fast[panel] < decivolts_min[panel])
            ovp_ok[panel] = 0;
        else
            ovp_ok[panel] = 1;

        if (!ovp_ok[panel]) disableChannel(panel);

        //-max current--------------------------------------------------------------------------------------------------
        //SerialUSB.print("read_microamps: ");
        //SerialUSB.print(read_microamps_fast[panel]);
        //SerialUSB.print("\n");
        //SerialUSB.print("max_microamps: ");
        //SerialUSB.print(microamps_max[panel]);
        //SerialUSB.print("\n");
        ovc_ok[panel] = (read_microamps_fast[panel] < microamps_max[panel]) ? 1 : 0;

        if (!ovc_ok[panel]) disableChannel(panel);
    }
}

void updateMinMax()
{
    for (int panel=0; panel<2; panel++) {
        decivolts_max[panel] = set_decivolts[panel] * max_voltage_scaler;
        microamps_max[panel] = set_microamps[panel] * max_current_scaler;
        decivolts_min[panel] = 0; // set_decivolts[panel] * min_voltage_scaler;
    }

}

void updateDacs()
{
    for (int ichan=0; ichan<2; ichan++) {
        if (!output_on[ichan]) { // turn off the output if the output is off. duh!
            disableChannel(ichan);
            controller.writeDac(ichan,   output_zerovolt_safe);
            controller.writeDac(ichan+2, output_zerovolt_safe); // TODO: for DAC loopback ONLY!!
            last_set_decivolts[ichan] = 0; // make sure this gets set here, or we won't write the dac when enabling the output
        }
        else if (set_decivolts[ichan]!=last_set_decivolts[ichan]) { // no point in re-writing the same voltage on the dac

            uint16_t counts          = deciVoltsToDacCounts(set_decivolts[ichan]);
            uint16_t inverted_counts = invertCounts (counts);

            last_set_decivolts[ichan]=set_decivolts[ichan];

            SerialUSB.print("setting dac ");
            SerialUSB.print(ichan);
            SerialUSB.print(" to ");
            SerialUSB.print(10*set_decivolts[ichan]);
            SerialUSB.print(" = (");
            SerialUSB.print(inverted_counts);
            SerialUSB.print(" inverted dac_counts) ");
            SerialUSB.print(" , (");
            SerialUSB.print(counts);
            SerialUSB.print(" non-inverted dac_counts)");
            SerialUSB.print("\n");

            controller.writeDac(ichan,   inverted_counts);
            controller.writeDac(ichan+2, inverted_counts);  // TODO: for dac loopback only
        }
    }
}

void checkStability() {

    int      stability_win_size = 20; // decivolts; use 2V for now.

    for (int i=0; i<2; i++) {

        uint16_t decivolts = countsToDeciVolts(read_voltage_counts_average[i]);

        if ( (set_decivolts[i] + stability_win_size > decivolts) &&
             (set_decivolts[i] - stability_win_size < decivolts)
           )
            status_ok[i] = 1;
        else
            status_ok[i] = 0;
    }
}

void fastReadAdcs() {

    //-Readings --------------------------------------------------------------------------------------------------------
    for (int i=0; i<8; i++) {
        adc_fast_reading[i] = controller.readArduinoAdc(i);
    }

    //-Smoothing--------------------------------------------------------------------------------------------------------
    for (int i=0; i<2; i++) {
        read_voltage_counts[i] = adc_fast_reading[i];
        read_current_counts[i] = adc_fast_reading[i+2];

        // update running sum --- push in new value, pop out the old
        read_voltage_counts_total[i] =read_voltage_counts_total[i] - read_voltage_counts_train [i][read_index] + read_voltage_counts[i];
        read_current_counts_total[i] =read_current_counts_total[i] - read_current_counts_train [i][read_index] + read_current_counts[i];

        // update this index in the ring buffer
        read_voltage_counts_train[i][read_index] = read_voltage_counts[i];
        read_current_counts_train[i][read_index] = read_current_counts[i];

        // averaging
        read_voltage_counts_average[i] = read_voltage_counts_total[i]/num_boxcars;
        read_current_counts_average[i] = read_current_counts_total[i]/num_boxcars;

        // physical units for min/max checking
        read_decivolts_fast [i] = countsToDeciVolts(read_voltage_counts[i]);
        read_microamps_fast [i] = countsToMicroAmps(read_current_counts[i]);

    }

    // ring buffer index
    read_index++;
    if (read_index >= num_boxcars) {
        read_index = 0;
    }

}
