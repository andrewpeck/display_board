#include "controller.h"
#include <SPI.h>
#include <CleO.h>

Controller controller;

#define MY_ORANGE   0xfa7626UL
#define MY_MAGENTA  0xff0086UL
#define MY_RED      0xff6666UL
#define MY_GREEN    0x89df87UL
#define MY_BLUE     0x007fc8UL
#define MY_DARKBLUE 0x004166UL
#define MY_PINK     0xe9d3ebUL
#define MY_BLACK    0x000000UL
#define MY_WHITE    0xffffffUL
#define MY_PURPLE   0xb3b3ccUL

int screenWidth = 480, screenHeight = 320;
int windowWidth = 240, windowHeight = 320;


int bounding_margin = 20;
uint32_t keypad_taglist=0;

uint16_t voltage_max [2] = {0,0};
uint16_t current_max [2] = {0,0};
uint16_t voltage_min [2] = {0,0};

float max_voltage_scaler = 1.1;
float min_voltage_scaler = 0.9;
float max_current_scaler = 1.1;

bool keypad_enabled [2] = {0,0};

uint32_t keypad_color       = MY_PURPLE;
uint32_t keypad_text_color  = MY_BLACK;

uint32_t display_text_color = MY_BLACK;

uint32_t background_color   = MY_PINK;

uint32_t line_color         = MY_DARKBLUE;

uint32_t color_good         = MY_GREEN;
uint32_t color_bad          = MY_RED;

uint32_t color_on           = MY_GREEN;
uint32_t color_off          = MY_RED;

uint32_t set_color          = keypad_color;
uint32_t touch_color        = MY_BLUE;

int16_t font;

enum {
    tag_null    , // 0
    tag_0       , // 1
    tag_1       , // 2
    tag_2       , // 3
    tag_3       , // 4
    tag_4       , // 5
    tag_5       , // 6
    tag_6       , // 7
    tag_7       , // 8
    tag_8       , // 9
    tag_9       , // 10
    tag_point   , // 11
    tag_left    , // 12
    tag_right   ,//  13

    tag_power0   , // 13
    tag_voltage0 , // 14
    tag_current0, // 15

    tag_power1   , // 13
    tag_voltage1 , // 14
    tag_current1  // 15
};

#define Serial SerialUSB

bool ovp_ok        = 1;
bool ovc_ok        = 1;
bool output_on [2] = {0,0};
bool status_ok     = 1;

int counter    = 0;
int dac_counts = 0;
float radians  = 0;

int last_dur = 0;

uint8_t   short_high_cnt       = 0;
uint8_t   short_low_cnt        = 0;
bool  short_press_allow    = 0;
bool  short_press_detected = 0;

uint8_t   long_high_cnt       = 0;
uint8_t   long_low_cnt        = 0;
bool  long_press_allow    = 0;
bool  long_press_detected = 0;

int16_t x, y, dur, current_tag;

uint16_t max_voltage = 0;
uint16_t max_current = 0;

uint16_t set_voltage [2]      = {0,0};
uint16_t set_current [2]      = {0,0};

uint16_t read_voltage [2]      = {0,0};
uint16_t read_current [2]      = {0,0};

const int num_readings = 10;
uint16_t read_voltage_arr [2][num_readings];
uint16_t read_current_arr [2][num_readings];
uint8_t  read_index = 0;
uint32_t read_voltage_total [2];
uint16_t read_voltage_average[2];
uint32_t read_current_total [2];
uint16_t read_current_average[2];

uint16_t last_set_voltage [2] = {0,0};
uint16_t last_set_current [2] = {0,0};

uint16_t set_voltage_buffer [2] = {0,0}; // set voltage (in VOLTS)
uint16_t set_current_buffer [2] = {0,0}; // set current (in CENTI-amps !!)

uint16_t adc_fast_reading [8];

bool voltage_set_mode = 0;
bool current_set_mode = 0;

int tag_powers   [2] = {tag_power0, tag_power1};
int tag_currents [2] = {tag_current0, tag_current1};
int tag_voltages [2] = {tag_voltage0, tag_voltage1};

uint8_t cursor_index     = 0;
uint8_t decimal_index    = 1;
uint8_t decimal_position = 2;

char set_voltage_str       [] = "    V";
char set_current_str       [] = "00.0mA";

char set_voltage_highlight [] = "    V";
char set_current_highlight [] = "00.0mA";

void setup () {

    for (int i=0; i<num_readings; i++) {
        read_voltage_arr [0][i] = 0;
        read_voltage_arr [1][i] = 0;
        read_current_arr [0][i] = 0;
        read_current_arr [1][i] = 0;
    }

    controller.enableDac();
    controller.enableSwitch();
    controller.setDacGain(2);
    analogReadResolution(16);

    pinMode(12, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(13, INPUT);

    digitalWrite(10, LOW);

    SerialUSB.begin(0);

    digitalWrite(10, HIGH);

    SerialUSB.println("UCLA Analog Display Board");

    SerialUSB.println("Starting SPI...");

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setBitOrder(MSBFIRST);

    SerialUSB.println("Started SPI...");

    SerialUSB.println("Setting DDS Frequency");

    for (int ichan=0; ichan<8; ichan++) {
        controller.writeDac(ichan, 0);
    }

    SerialUSB.println("DDS Frequency Set");


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

int last_time =0;

void loop () {


    //------------------------------------------------------------------------------------------------------------------
    // Diagnostic Printouts
    //------------------------------------------------------------------------------------------------------------------

    if (1==0)
        if (current_tag!=0 && current_tag!=255) {
            SerialUSB.print("tag: ");
            SerialUSB.print(current_tag);
            SerialUSB.print(" dur: ");
            SerialUSB.print(dur);
            SerialUSB.print(" x: ");
            SerialUSB.print(x);
            SerialUSB.print(" y: ");
            SerialUSB.print(y);
            SerialUSB.print("\n");
        }

    if (counter==0) {SerialUSB.println("starting loop");};


    if ((counter%1000)==0) {

        SerialUSB.println("beat");
        SerialUSB.println((millis()-last_time) / 1000);

        last_time = millis();

        for (int panel=0; panel<2; panel++) {
            SerialUSB.print("panel ");
            SerialUSB.print(panel);
            SerialUSB.print(" voltage is set to ");
            SerialUSB.print(set_voltage[panel]);
            SerialUSB.print("\n");
        }

        // ovp_ok = !ovp_ok;
        // ovc_ok = !ovc_ok;
        // status_ok = !status_ok;

    }

    //------------------------------------------------------------------------------------------------------------------
    // Read ADCs
    //------------------------------------------------------------------------------------------------------------------

    for (int i=0; i<8; i++) {
        adc_fast_reading[i] = controller.readArduinoAdc(i);
    }
    for (int i=0; i<2; i++) {
        read_voltage[i] = adc_fast_reading[i];
        read_current[i] = adc_fast_reading[i+2];

        read_voltage_arr[i][read_index] = adc_fast_reading[i];
        read_current_arr[i][read_index] = adc_fast_reading[i+2];

        read_voltage_total[i] =read_voltage_total[i] - read_voltage_arr [i][read_index] + read_voltage[i];
        read_current_total[i] =read_current_total[i] - read_current_arr [i][read_index] + read_current[i];

        read_voltage_average[i] = read_voltage_total[i]/num_readings;
        read_current_average[i] = read_current_total[i]/num_readings;

    }
    read_index++;
    if (read_index >= num_readings) {
    read_index = 0;
    }



    checkMinMax();

    //------------------------------------------------------------------------------------------------------------------
    // Update DAC
    //------------------------------------------------------------------------------------------------------------------

    if (counter%1==0) {
        updateDacs();
    }


    if (counter%5==0) {
        updateScreen();
    }

    updateMinMax();

    //------------------------------------------------------------------------------------------------------------------
    // Increment Loop Counter
    //------------------------------------------------------------------------------------------------------------------

    counter++;
}

void checkMinMax()
{
    for (int panel=0; panel<2; panel++) {

        if (read_voltage[panel] > voltage_max[panel])
            ovp_ok = 0;
        if (read_voltage[panel] < voltage_min[panel])
            ovp_ok = 0;
        else
            ovp_ok = 1;
        //------------------------------------------------------------------------------------------------------------------
        if (read_current[panel] > current_max[panel])
            ovc_ok = 0;
        else
            ovc_ok = 1;

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
            controller.writeDac(ichan, 0);
        }
        else if (set_voltage[ichan]!=last_set_voltage[ichan]) { // no point in re-writing the same voltage on the dac

            int voltage = (int) set_voltage[ichan%2] * 1.5;

            last_set_voltage[ichan]=set_voltage[ichan];

            SerialUSB.print("setting dac ");
            SerialUSB.print(ichan);
            SerialUSB.print(" to ");
            SerialUSB.print(voltage);
            SerialUSB.print("\n");

            controller.writeDac(ichan, voltage);
        }
    }
}

void updateScreen()
{
    //------------------------------------------------------------------------------------------------------------------
    // Start Drawing Screen
    //------------------------------------------------------------------------------------------------------------------

    CleO.Start();

    //------------------------------------------------------------------------------------------------------------------
    // Build Keypads / Status Screens
    //------------------------------------------------------------------------------------------------------------------

    CleO.RectangleJustification(MM);
    CleO.LineColor(line_color);

    if (keypad_enabled[0]) {
        buildStatus(0);
        buildKeypad(0);
    } else if (keypad_enabled[1]) {
        buildStatus(1);
        buildKeypad(1);
    } else {
        buildStatus(0);
        buildStatus(1);
    }

    CleO.LineWidth(3);
    CleO.Line(windowWidth, 0, windowWidth, windowHeight); // center line vertical

    //------------------------------------------------------------------------------------------------------------------
    // Collect Tags
    //------------------------------------------------------------------------------------------------------------------

    processButtons();

    //------------------------------------------------------------------------------------------------------------------
    // Parse Tags
    //------------------------------------------------------------------------------------------------------------------

    // long press for power and opening the keypad
    if (long_press_detected) {
        processLongPress();
    }

    // short presses are OK for keypad touches
    else if (short_press_detected && (keypad_enabled[0] || keypad_enabled[1])) {
        processShortPress();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Update Screen
    //------------------------------------------------------------------------------------------------------------------

    CleO.Show();
}

void setDecimalPoint()
{
}

void moveCursorLeft()
{
    if (current_set_mode && cursor_index==3) // skip the decimal place
        cursor_index--;

    if (cursor_index>0)
        cursor_index--;
}

void moveCursorRight()
{
    int max=3;

    if (current_set_mode && cursor_index==1) // skip the decimal place
        cursor_index++;

    if (cursor_index<max)
        cursor_index++;
    else
        cursor_index=0;
}

void setValue (uint8_t value, uint8_t panel)
{
    // take value
    // convert to string
    char thischar [] = " ";
    sprintf(thischar,   "%1d",  value);

    char* end;

    updateSetStrings(panel);

    if (voltage_set_mode) {
        SerialUSB.print("Read in character: " );
        SerialUSB.print(thischar[0]);
        SerialUSB.print("\n" );

        SerialUSB.print("Old string ");
        SerialUSB.print(set_voltage_str);
        SerialUSB.print("\n");

        SerialUSB.print("Cursor index ");
        SerialUSB.print(cursor_index);
        SerialUSB.print("\n");

        set_voltage_str[cursor_index] = thischar[0]; // swap this index

        SerialUSB.print("New string ");
        SerialUSB.print(set_voltage_str);
        SerialUSB.print("\n");

        set_voltage_buffer[panel]     = strtol(set_voltage_str, &end, 10); // convert to value

        SerialUSB.print("Setting voltage buffer to ");
        SerialUSB.print(set_voltage_buffer[panel]);
        SerialUSB.print("\n");

        moveCursorRight();
    }
    else if (current_set_mode) {

        set_current_str[cursor_index]     = thischar[0];
        set_current_str[decimal_position] = ' '; // convert the decimal to a space so strol will parse it as two numbers

        char* end;
        int characteristic        = strtol(set_current_str, &end, 10);
        int mantissa              = strtol(end,             &end, 10);

        set_current_buffer[panel] = 10*characteristic + mantissa;

        SerialUSB.print("Setting current buffer to ");
        SerialUSB.print(set_current_buffer[panel]);
        SerialUSB.print("\n");

        moveCursorRight();
    }
}

void buildStatus (int panel)
{

    int x_offset=0;
    if (panel==1) x_offset=240;

    //------------------------------------------------------------------------------------------------------------------
    // Draw Background Boxes
    //------------------------------------------------------------------------------------------------------------------

    // over voltage
    CleO.RectangleColor(ovp_ok ? color_good : color_bad);
    CleO.RectangleXY(x_offset+200, 40, 80,80);

    // over current
    CleO.RectangleColor(ovc_ok ? color_good : color_bad);
    CleO.RectangleXY(x_offset+200, 120, 80,80);

    // on/off
    CleO.RectangleColor(output_on[panel] ? color_on : color_off);
    CleO.RectangleXY(x_offset+60, 280, 120,80);

    // status
    CleO.RectangleColor(status_ok ? color_good : color_bad);
    CleO.RectangleXY(x_offset+180, 280, 120,80);

    // voltage set
    CleO.RectangleColor(voltage_set_mode ? set_color : background_color);
    CleO.RectangleXY(x_offset+60, 200, 120,80);

    // current set
    CleO.RectangleColor(current_set_mode ? set_color : background_color);
    CleO.RectangleXY(x_offset+180, 200, 120,80);


    //------------------------------------------------------------------------------------------------------------------
    // Draw Divider Lines
    //------------------------------------------------------------------------------------------------------------------

    // Status Lines
    CleO.Line(0   + x_offset, 80  , 240 + x_offset, 80);  // A
    CleO.Line(0   + x_offset, 160 , 240 + x_offset, 160); // B
    CleO.Line(0   + x_offset, 240 , 240 + x_offset, 240); // C
    CleO.Line(0   + x_offset, 320 , 240 + x_offset, 320); // D
    CleO.Line(160 + x_offset, 0   , 160 + x_offset, 160); // E
    CleO.Line(120 + x_offset, 160 , 120 + x_offset, 320); // F


    //------------------------------------------------------------------------------------------------------------------
    // Update Text
    //------------------------------------------------------------------------------------------------------------------

    int mv = (33000 * read_voltage_average[panel]) / (65535); // rescale to millivolts, 0-5000

    int ma = (33000 * read_current_average[panel]) / (65535); // rescale to millivolts, 0-5000

    //SerialUSB.println(mv);
    char buf_volts[10];
    char buf_current[10];

    sprintf(buf_volts,   "%4d.%1dV",  mv / 10, mv % 10);
    sprintf(buf_current, "%1d.%1dmA", ma / 100, ma % 100);

    CleO.LineWidth(1);
    CleO.StringExt(FONT_SANS_3 , 80  + x_offset , 40  , display_text_color , MM , 0 , 0 , buf_volts); // voltage

    CleO.StringExt(FONT_SANS_3 , 80  + x_offset , 120 , display_text_color , MM , 0 , 0 , buf_current); // current

    CleO.StringExt(FONT_SANS_3 , 200 + x_offset , 40  , display_text_color , MM , 0 , 0 , "OVP");  // Overvoltage
    CleO.StringExt(FONT_SANS_3 , 200 + x_offset , 120 , display_text_color , MM , 0 , 0 ,"OVC");  // Overcurrent



    char *output_str [2] = {(char*)"OFF", (char*)"ON"};
    CleO.StringExt(FONT_SANS_3 , 60  + x_offset , 280 , display_text_color , MM , 0 , 0 , output_str[output_on[panel]]); // On/Off

    CleO.StringExt(FONT_SANS_3 , 180 + x_offset , 280 , display_text_color , MM , 0 , 0 , "STAT"); // Status


    //------------------------------------------------------------------------------------------------------------------
    // Voltage & Current Setting
    //------------------------------------------------------------------------------------------------------------------

    updateSetStrings(panel);

    // Cursor highlighting in voltage/current set mode

    if (keypad_enabled[panel] && (voltage_set_mode || current_set_mode) ) {


        for (int i=0; i<(4+current_set_mode); i++) { // current_set_mode string is 1 character longer
            if (i==cursor_index) {
                set_voltage_highlight[i] = set_voltage_str[cursor_index];
                set_current_highlight[i] = set_current_str[cursor_index];
            }
            else {
                set_voltage_highlight[i] = ' ';
                if (i!=decimal_position) set_current_highlight[i] = ' ';
            }
        }

        if (voltage_set_mode) set_voltage_str[cursor_index] = ' ';
        if (current_set_mode) set_current_str[cursor_index] = ' ';


        if      (voltage_set_mode) CleO.StringExt(FONT_VGA_16 , 60  + x_offset , 200 , touch_color , MM , 0 , 0 , set_voltage_highlight); // voltage set
        else if (current_set_mode) CleO.StringExt(FONT_VGA_16 , 180 + x_offset , 200 , touch_color , MM , 0 , 0 , set_current_highlight); // current set
    }

    CleO.StringExt(FONT_VGA_16 , 60  + x_offset , 200 , display_text_color , MM , 0 , 0 , set_voltage_str); // voltage set
    CleO.StringExt(FONT_VGA_16 , 180 + x_offset , 200 , display_text_color , MM , 0 , 0 , set_current_str); // current set

}

void updateSetStrings (uint8_t panel) {

    // need to breakdown the set current for decimal representation
    int characteristic = set_current_buffer[panel] / 10;
    int mantissa       = set_current_buffer[panel] % 10;

    sprintf(set_voltage_str,         "%04luV",        set_voltage_buffer[panel]);
    sprintf(set_current_str,         "%02lu.%01lumA",  characteristic, mantissa);
}

void processButtons () { 

    CleO.TouchCoordinates(x, y, dur, current_tag);

    // manual tagging for >14 --- the software is REALLY buggy otherwise
    if (current_tag > 13)  {
        for (int window=0; window<2; window++) {
            if (!keypad_enabled[!window] && y>160 && x<240+window*windowWidth) { // We DONT want to parse this screen as buttons if the other side is using it as a keypad

                //--------------------------------------------------------------------------------------------------
                // Voltage Set Mode
                //--------------------------------------------------------------------------------------------------

                //  top         left     right       bottom
                if (      y > 160+10
                        &&    y < 240-10
                        &&    x > window*windowWidth + 0+10
                        &&    x < window*windowWidth + 120-10 
                   )
                {
                    current_tag = tag_voltages[window];
                }

                //--------------------------------------------------------------------------------------------------
                // Current Set Mode
                //--------------------------------------------------------------------------------------------------

                else if (
                        y > 160+10
                        && y < 240-10
                        && x > window*windowWidth+120 +10
                        && x < window*windowWidth+240-10
                        )
                {
                    current_tag = tag_currents[window];
                }

                //--------------------------------------------------------------------------------------------------
                // Power Toggle
                //--------------------------------------------------------------------------------------------------

                else if (
                        y > 240+10
                        && x >  window*windowWidth + 0  +10
                        && x <  window*windowWidth + 120-10
                        && y < 320-10
                        )
                {
                    current_tag = tag_powers[window];
                }
            }
        }
    }

    // press (1  , &short_high_cnt , &short_low_cnt , &short_press_allow , &short_press_detected);
    shortPress (&short_press_detected);
    press (25 , &long_high_cnt  , &long_low_cnt  , &long_press_allow  , &long_press_detected);

    last_dur = dur;
}

void shortPress (bool *press_detected)
{
    if (dur==1 && last_dur==0 && !*press_detected) {
        *press_detected = 1;
        SerialUSB.println("Short press!");
    }
    else {
        *press_detected = 0;
    }

}

void press (uint8_t debounce_time, uint8_t *high_cnt, uint8_t *low_cnt, bool *press_allow, bool *press_detected)
{
    if (dur==1 && last_dur==1 && *press_allow) {
        *high_cnt += 1;
    }

    if (dur==0 && last_dur==0 && !*press_allow)  {
        *low_cnt += 1;
    }

    if (*low_cnt > debounce_time/2) {
        *press_allow = 1;
        *low_cnt     = 0;
        *high_cnt    = 0;
    }

    if (*press_allow && *high_cnt > debounce_time) {
        SerialUSB.println("Long press!");
        *press_allow    = 0;
        *press_detected = 1;
    }
    else
    {
        *press_detected = 0;
    }
}


void buildKey    (int x, int y, int box_size, int tag, char* text) {

    bool tagged = (current_tag==tag);

    int tag_size = box_size-(tagged ? 0 : bounding_margin);
    int x_size   = tag_size;
    int y_size   = (tag==tag_left||tag==tag_right) ? tag_size/2 : tag_size;

    CleO.Tag(tag);

    CleO.RectangleColor(tagged ? touch_color : keypad_color);
    CleO.RectangleXY(x, y, x_size, y_size);

    //-the dot is a special case----------------------------------------------------------------------------------------
    if (tag==tag_point) {
        int dot_size = 4;
        CleO.RectangleColor(keypad_text_color);
        CleO.RectangleXY(x, y, dot_size, dot_size);
    }
    //-the arrows are also special cases--------------------------------------------------------------------------------
    else if (tag==tag_left) {
        int needle_length = 25;
        int needle_radius = 0;
        CleO.NeedleWidth (10);

        CleO.NeedleExt(x+needle_length/4, y, needle_length, keypad_text_color, 90,  needle_radius);
    }
    //-the arrows are also special cases--------------------------------------------------------------------------------
    else if (tag==tag_right) {
        int needle_length = 25;
        int needle_radius = 0;
        CleO.NeedleWidth (10);

        CleO.NeedleExt(x-needle_length/4, y, needle_length, keypad_text_color, 270, needle_radius);
    }
    //-normal characters are normal-------------------------------------------------------------------------------------
    else {
        CleO.StringExt(FONT_SANS_4 , x,y, keypad_text_color , MM , 0 , 0 , text);
    }
}




void buildKeypad (int keypad)
{

    int x_offset=0;
    if (keypad==0)
        x_offset = windowWidth;


    CleO.LineWidth(1);

    /* Draw a rectangle at (160, 240) point with 200 width, 400 height */


    //------------------------------------------------------------------------------------------------------------------
    // Background Color
    //------------------------------------------------------------------------------------------------------------------
    CleO.RectangleColor(keypad_color);
    CleO.RectangleXY(x_offset+windowWidth/2, 0+windowHeight/2, 240, 320);

    //------------------------------------------------------------------------------------------------------------------
    // Build each key
    //------------------------------------------------------------------------------------------------------------------

    buildKey(40   + x_offset , 40  , 80 , tag_7 , (char*)"7");
    buildKey(120  + x_offset , 40  , 80 , tag_8 , (char*)"8");
    buildKey(200  + x_offset , 40  , 80 , tag_9 , (char*)"9");
    buildKey(40   + x_offset , 120 , 80 , tag_4 , (char*)"4");
    buildKey(120  + x_offset , 120 , 80 , tag_5 , (char*)"5");
    buildKey(200  + x_offset , 120 , 80 , tag_6 , (char*)"6");
    buildKey(40   + x_offset , 200 , 80 , tag_1 , (char*)"1");
    buildKey(120  + x_offset , 200 , 80 , tag_2 , (char*)"2");
    buildKey(200  + x_offset , 200 , 80 , tag_3 , (char*)"3");
    buildKey(40   + x_offset , 280 , 80 , tag_0 , (char*)"0");
    buildKey(120  + x_offset , 280 , 80 , tag_point , (char*)".");

    buildKey(200  + x_offset , 260 , 80 , tag_left  , (char*)"l");
    buildKey(200  + x_offset , 300 , 80 , tag_right , (char*)"r");

    //------------------------------------------------------------------------------------------------------------------
    // Draw Dividing Lines
    //------------------------------------------------------------------------------------------------------------------

    // Status Lines
    CleO.Line(x_offset+80  , 0   , x_offset+80  , 320);
    CleO.Line(x_offset+160 , 0   , x_offset+160 , 320);
    CleO.Line(x_offset+0   , 80  , x_offset+240 , 80);
    CleO.Line(x_offset+0   , 160 , x_offset+240 , 160);
    CleO.Line(x_offset+0   , 240 , x_offset+240 , 240);

    CleO.Line(x_offset+160   , 280 , x_offset+240 , 280);

}

void processLongPress() {
    for (int panel=0; panel<2; panel++) {
        if (current_tag==tag_powers[panel]) {
            output_on[panel] = !output_on[panel];
        }
        else if (current_tag==tag_voltages[panel] && !current_set_mode) // can't switch
        {
            SerialUSB.println("Toggling Voltage Set Mode");
            keypad_enabled[panel] = !keypad_enabled[panel];
            voltage_set_mode      = !voltage_set_mode;

            // update the set voltage when we leave set mode; keep the value in a buffer before that
            if (!voltage_set_mode) {
                set_voltage[panel] = set_voltage_buffer[panel];
                SerialUSB.print("Setting panel ");
                SerialUSB.print(panel);
                SerialUSB.print(" voltage to ");
                SerialUSB.print(set_voltage[panel]);
                SerialUSB.print("\n");
            }

            cursor_index          = 0;
        }
        else if (current_tag==tag_currents[panel] && !voltage_set_mode)
        {
            SerialUSB.println("Toggling Current Set Mode");
            keypad_enabled[panel] = !keypad_enabled[panel];
            current_set_mode      = !current_set_mode;
            cursor_index          = 0;

            // update the set current when we leave set mode; keep the value in a buffer before that
            if (!current_set_mode) {
                set_current[panel] = set_current_buffer[panel]; 
                SerialUSB.print("Setting panel ");
                SerialUSB.print(panel);
                SerialUSB.print(" current to ");
                SerialUSB.print(set_current[panel]);
            }
        }
    }

}

void processShortPress() {
    switch (current_tag) {
        case tag_0     : setValue(0x0, 0x1&keypad_enabled[1]) ; break ;
        case tag_1     : setValue(0x1, 0x1&keypad_enabled[1]) ; break ;
        case tag_2     : setValue(0x2, 0x1&keypad_enabled[1]) ; break ;
        case tag_3     : setValue(0x3, 0x1&keypad_enabled[1]) ; break ;
        case tag_4     : setValue(0x4, 0x1&keypad_enabled[1]) ; break ;
        case tag_5     : setValue(0x5, 0x1&keypad_enabled[1]) ; break ;
        case tag_6     : setValue(0x6, 0x1&keypad_enabled[1]) ; break ;
        case tag_7     : setValue(0x7, 0x1&keypad_enabled[1]) ; break ;
        case tag_8     : setValue(0x8, 0x1&keypad_enabled[1]) ; break ;
        case tag_9     : setValue(0x9, 0x1&keypad_enabled[1]) ; break ;
        case tag_point : setDecimalPoint()                  ; break ;
        case tag_left  : moveCursorLeft()                   ; break ;
        case tag_right : moveCursorRight()                  ; break ;
    }
}
