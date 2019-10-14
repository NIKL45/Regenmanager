#include <Arduino.h>
#include <HCSR04.h>
#include "U8glib.h"

UltraSonicDistanceSensor hcsr04(12, 11); //(trig, echo)

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // A4-SDA & A5-SCL

//OLED.setFont(u8g_font_unifont); // original
//OLED.setFont(u8g_font_osb21);   // extrem groß "Hello Wor" rest fehlt
//OLED.setFont(u8g_font_helvR14); // Helvetica sehr groß
//OLED.setFont(u8g_font_helvR12); // Helvetica groß
//OLED.setFont(u8g_font_helvR10); // Helvetica relativ groß
//OLED.setFont(u8g_font_helvR08); // Helvetica mittel
//OLED.setFont(u8g_font_04b_03r); // Klein
//OLED.setFont(u8g_font_04b_03br); // Mini

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int dist;
const int minDist = 50;  // cm
const int maxDist = 200; // cm
int percent;

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

void setup()
{
    u8g.begin();
    //Serial.begin(9600);
}

void loop()
{
    dist = hcsr04.measureDistanceCm();
    percent = calcPercent(dist, minDist, maxDist);
    if (dist > 5)
    {
        //Serial.println(dist);

        u8g.firstPage();
        do
        {
            u8g.setFont(u8g_font_osb29);

            char tmp_string[10];
            itoa(percent, tmp_string, 10);
            u8g.drawStr(27, 45, tmp_string);
        } while (u8g.nextPage());
    }

    delay(200);
}
