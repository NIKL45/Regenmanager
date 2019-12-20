#include <Arduino.h>
#include <HCSR04.h>
#include "U8glib.h"
#include "OneButton.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Zpin 5
#define TWpin 4
#define RelayTime 10
#define SysOpin 3
#define BuzzerPin 9
#define ButtonPin A2

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UltraSonicDistanceSensor hcsr04(11, 12); //(trig, echo)

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // A4-SDA & A5-SCL

OneButton button(ButtonPin, true);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long preMillis;
int dist;
bool mute = false;
bool alarm = false;
bool mode = false;
bool lastMode = mode;
const int minDist = 30;  // cm
const int maxDist = 280; // cm
int percent;
int lastPercent = 120; //> 100%
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
            if (alarm)
            {
                if ((millis() - preMillis) > 1500)
                {
                    u8g.setColorIndex(1);
                    u8g.drawBox(0, 0, 128, 64);
                    u8g.setColorIndex(0);
                    u8g.drawStr(14, 16, "! Ueberlauf !");
                    if ((millis() - preMillis) > 3000)
                    {
                        preMillis = millis();
                    }
                }
                else
                {
                    u8g.setColorIndex(1);
                    u8g.drawStr((128 - 100) / 2, 16, "Trinkwasser");
                }
            }
            else
            {
                u8g.setColorIndex(1);
                u8g.drawStr((128 - 100) / 2, 16, "Trinkwasser");
            }
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
            digitalWrite(Zpin, HIGH);
            delay(RelayTime);
            digitalWrite(Zpin, LOW);
        }
        else
        {
            digitalWrite(TWpin, HIGH);
            delay(RelayTime);
            digitalWrite(TWpin, LOW);
        }

        lastMode = mode;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Mute()
{
    mute = true;
    noTone(BuzzerPin);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Reset()
{
    alarm = false;
    mute = false;
    noTone(BuzzerPin);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckAlarm()
{
    if (alarm)
    {
        if (!mute)
        {
            if ((millis() - preMillis) > 2650)
            {
                tone(BuzzerPin, 3200);
            }
            else
            {
                noTone(BuzzerPin);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{

    pinMode(TWpin, OUTPUT);
    pinMode(Zpin, OUTPUT);
    pinMode(BuzzerPin, OUTPUT);
    pinMode(SysOpin, INPUT_PULLUP);
    pinMode(ButtonPin, INPUT_PULLUP);

    button.attachClick(Mute);
    button.attachLongPressStart(Reset);

    //attachInterrupt(digitalPinToInterrupt(3), systemOverflow, FALLING);

    digitalWrite(TWpin, LOW);
    digitalWrite(Zpin, HIGH);  //
    delay(RelayTime);         // als erstes immer auf Zisterne schalten
    digitalWrite(Zpin, LOW); //

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
    //if (dist < 10 || (percent - lastPercent) > 5 ) 
    if (dist < 10) 
    {
        digitalWrite(13, HIGH);
        delay(2000);
        digitalWrite(13, LOW);
        goto a;
    }

    /////////////////////////////////////////////// TW / Z //////////////////////////////////////////////////////

    if (!alarm)
    {

        if (percent <= 10)
        {
            mode = true;
        }
        else if (percent >= 15)
        {
            mode = false;
        }
    }
    else
    {
        mode = true;
    }
    /////////////////////////////////////////////// SystemOverflow //////////////////////////////////////////////

    if (digitalRead(SysOpin) == LOW)
    {
        alarm = true;
    }
    /*     else
    {
        alarm = false;
        mute = false;
    } */

    CheckAlarm();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Serial.println(dist);

    button.tick();

    OLEDprint();

    updatePumpMode();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    lastPercent = percent; 
    delay(2000);
}