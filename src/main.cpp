#include <Arduino.h>
#include <HCSR04.h>
#include "U8glib.h"

UltraSonicDistanceSensor hcsr04(12, 11); //(trig, echo)

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // A4-SDA & A5-SCL

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Zpin 5
#define TWpin 4

int dist;
bool mode = false;
bool lastMode = mode;
const int minDist = 40;  // cm
const int maxDist = 280; // cm
int percent;
const char percSymb[2] = "%";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int getDist(int count)
{
    int readings[count];
    int readIndex = 0;
    int total = 0;

    total = total - readings[readIndex];
    readings[readIndex] = hcsr04.measureDistanceCm();
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= count)
        readIndex = 0;
    int d = total / count;

    return d;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int calcPercent(int d, int min, int max)
{
    int perc = 0;
    perc = map(d, min, max, 100, 0);
    if (perc < 0)
    {
        perc = 0;
    }

    return perc;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OLEDprint()
{
    u8g.firstPage();
    do
    {
        u8g.setFont(u8g_font_helvR14);
        if (mode == false)
        {
            u8g.drawStr((128 - 67) / 2, 16, "Zisterne");
        }
        else
        {
            u8g.drawStr((128 - 100) / 2, 16, "Trinkwasser");
        }

        u8g.setFont(u8g_font_osb29);
        char tmp_string[10];
        itoa(percent, tmp_string, 10);
        strcat(tmp_string, percSymb);
        //Serial.println(dist);
        u8g.drawStr((128 - u8g.getStrWidth(tmp_string)) / 2, 55, tmp_string);
    } while (u8g.nextPage());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void updatePumpMode()
{
    if (mode != lastMode)
    {

    if (mode == false)
    {
        digitalWrite(Zpin, LOW);
        delay(100);
        digitalWrite(Zpin, HIGH);
    }
    else
    {
        digitalWrite(TWpin, LOW);
        delay(100);
        digitalWrite(TWpin, HIGH);
    }

    lastMode = mode;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
    pinMode(TWpin, OUTPUT);
    pinMode(Zpin, OUTPUT);

    digitalWrite(TWpin, HIGH);
    digitalWrite(Zpin, LOW);  //
    delay(10);                // als erstes immer auf Zisterne schalten
    digitalWrite(Zpin, HIGH); //

    //Serial.begin(9600);

    u8g.begin();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
a:
    dist = hcsr04.measureDistanceCm();
    percent = calcPercent(dist, minDist, maxDist);
    if (dist < 10)
    {
        delay(150);
        goto a;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (percent <= 10)
    {
        mode = true;
    }
    else
    {
        mode = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Serial.println(dist);
    OLEDprint();

    updatePumpMode();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    delay(200);
}