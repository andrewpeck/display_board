#include "controller.h"
#include <SPI.h>
#include <CleO.h>

Controller controller;

#define Serial SerialUSB

int16_t voltm, voltmc;

void setup () {

    SerialUSB.begin(0);


    while (!Serial) ;

    pinMode(3,OUTPUT);
    controller.enableDac();
    controller.enableSwitch();
    controller.setDacGain(1);
    analogReadResolution(12);

    SerialUSB.println("UCLA Analog Display Board");

    SerialUSB.println("Starting SPI...");


    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setBitOrder(MSBFIRST);
    // SPI.setDataMode(SPI_MODE0); // works with dds?
    // SPI.setDataMode(SPI_MODE2);  // works with dac
    // SPI.setDataMode(SPI_MODE0);  // adc

    SerialUSB.println("Started SPI...");

    SerialUSB.println("Setting DDS Frequency");

    for (int ichan=0; ichan<8; ichan++) {
        controller.writeDac(ichan, (1<<12)-1);
    }

    SerialUSB.println("DDS Frequency Set");


    // SerialUSB.println("Starting Cleo");

    // controller.disableCleo();
    // controller.enableCleo();

    //    /* Initialize CleO - needs to be done only once */
    //    CleO.begin();

    //    SerialUSB.println("Started CleO"); 


    //    SerialUSB.println("Rotating Display"); 
    //    CleO.DisplayRotate(2, 0);
    //    
    //    SerialUSB.println("Loading Image File 1"); 
    //    voltm = CleO.LoadImageFile("@Pictures/voltm35.jpg", 0);

    //    SerialUSB.println("Loading Image File2"); 
    //    voltmc = CleO.LoadImageFile("@Pictures/voltmc35.jpg", 0);
    //    SerialUSB.println("Set Background Gradient"); 
    //    CleO.SetBackgroundGradient(0, 0, 480, 320, LIGHT_BLUE, WHITE);

}


int counter=0; 
float radians = 0;
void loop () {

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    // pinMode(3,OUTPUT);
    // digitalWrite(3,HIGH);

    if (counter%2==0) { digitalWrite(5,HIGH); digitalWrite(4,HIGH); delay(10);}
    else              { digitalWrite(5,LOW ); digitalWrite(4, LOW); delay(10);}

    if (counter==0) {SerialUSB.println("starting loop");};

    counter++; 

    if (counter%(1<<19)==0) {
        SerialUSB.println("beat");
    }

    // DDS
          //  if (SerialUSB.available()) {
          //      //auto loopback = SerialUSB.readString();
          //      int frequency = SerialUSB.parseInt();
          //      SerialUSB.println(frequency, DEC);
          //      controller.setDDSFrequency(frequency);
          //  }

    // DAC
    if (SerialUSB.available()) {
        //auto loopback = SerialUSB.readString();
        int channel = SerialUSB.parseInt();
        int value   = SerialUSB.parseInt();

        if (value!=0) {
            SerialUSB.println("Receiving Serial Data");
            SerialUSB.println(value, DEC);
            SerialUSB.println(value);
            SerialUSB.println("Writing DAC");

            if (channel==0) {
                for (int ichan=0; ichan<8; ichan++) {
                    controller.writeDac(ichan, value);
                }
            }
            else {
                controller.writeDac(channel-1, value);
            }

            for (int ichan=0; ichan<8; ichan++) {
                SerialUSB.print(controller.readArduinoAdc(ichan),DEC);
                SerialUSB.print("  ");
            }
            SerialUSB.print("\n");
            for (int ichan=0; ichan<8; ichan++) {
                SerialUSB.print(controller.readSwitchedAdc(ichan, 10),DEC);
                SerialUSB.print("  ");
            }
            SerialUSB.print("\n");

            for (int ichan=0; ichan<8; ichan++) {
                SerialUSB.print(controller.readSwitchedAdc(ichan, 5),DEC);
                SerialUSB.print("  ");
            }
            SerialUSB.print("\n");

            for (int ichan=0; ichan<8; ichan++) {
                SerialUSB.print(controller.readSwitchedAdc(ichan, 2),DEC);
                SerialUSB.print("  ");
            }
            SerialUSB.print("\n");

            for (int ichan=0; ichan<8; ichan++) {
                SerialUSB.print(controller.readSwitchedAdc(ichan, 1),DEC);
                SerialUSB.print("  ");
            }
            SerialUSB.print("\n");
        }
    }

    // DAC Sin Wave
    //  if (radians>=6.28) radians=0;
    //  else               radians += 10*0.0174533;

    //  int value = abs(sin(radians)) * 4095;
    //  for (int ichan=0; ichan<8; ichan++) {
    //      controller.writeDac(ichan, value);
    //  }

    // SerialUSB.println("attempting SPI transfer");
    // SPI.transfer(0xFF);

    //    for (int ichan=0; ichan<8; ichan++) {
    //        uint8_t value = controller.readArduinoAdc(ichan);
    //        controller.writeDac(ichan, value);
    //    }

    //  int f = FONT_MEDIUM;            // Font for numbers
    //  int analog = analogRead(0);     // sample analog pin A0
    //  int angle = gauge_angle(analog);
    //  int cx = 240;                   // X center
    //  int cy = 344;                   // Y Center
    //  int r = 265;               // Gauge radius
    //  int lineLength;

    /* Start building a screen frame */

    //     SerialUSB.println("Cleo.Start()");
    //    CleO.Start();

    //    SerialUSB.println("Cleo.LineColor"); 
    //    CleO.LineColor(BLACK);

    //    /* Draw arcs with 30 lines */
    //    for (int i = 0; i <= 30; i++)
    //    {
    //      if (i % 5 != 0) {
    //        lineLength = 20;
    //        CleO.LineWidth(1);
    //      } else {
    //        lineLength = 40;
    //        CleO.LineWidth(2);
    //      }

    //      CleO.LineR1R2(cx, cy, r + lineLength, r, 135 + i * 90.00 / 30);
    //    }

    //    /* Draw V logo with two lines below it */
    //    CleO.StringExt(FONT_HUGE, cx, cy - 175, BLACK, MM, 0, 0, "V");
    //    CleO.LineExt(cx - 28, cy - 130, 60, BLACK, 270, 0);
    //    CleO.LineExt(cx - 28, cy - 118, 12, BLACK, 270, 0);
    //    CleO.LineExt(cx - 5, cy - 118, 12, BLACK, 270, 0);
    //    CleO.LineExt(cx + 20, cy - 118, 12, BLACK, 270, 0);

    //    // 0v and 5v markers
    //    CleO.StringExt(f, cx, cy, BLACK, MM, gauge_angle(0), r + 60, "0");
    //    CleO.StringExt(f, cx, cy, BLACK, MM, gauge_angle(1023), r + 60, "5");

    //    /* Select the scissor area */
    //    CleO.Scissor(0, 250, 480, 320);

    //    // Gauge background plate
    //    CleO.Bitmap(voltm, 0, 0);

    //    /* Select the scissor area */
    //    CleO.Scissor(0, 0, 480, 320);

    //    // Shadow
    //    CleO.NeedleWidth(15);
    //    CleO.NeedleColor(GRAY70);
    //    CleO.NeedleXY(cx, cy, r, angle);

    //    // Red needle
    //    CleO.NeedleWidth(12);
    //    CleO.NeedleColor(ORANGERED);
    //    CleO.NeedleXY(cx, cy, r, angle);

    //    // Pale red highlight
    //    CleO.NeedleWidth(5);
    //    CleO.NeedleColor(0xffc0c0);
    //    CleO.NeedleXY(cx, cy, r, angle);

    //    // Gauge cover plate
    //    CleO.BitmapJustification(BL);
    //    CleO.Bitmap(voltmc, 0, 320);

    //    /* Display completed screen frame */
    //    CleO.Show();
}








// Rescale x from the range (x0, x1) to the range (y0, y1)
float rescale(int x, int x0, int x1, int y0, int y1) {
    x = max(x0, min(x1, x));
    return y0 + ((x - x0) * long(y1 - y0) / float(x1 - x0));
}

float gauge_angle(int x) {
    return rescale(x, 0, 1023, 180 - 45, 180 + 44);
}

