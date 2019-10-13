#include <Arduino.h>
#include <HCSR04.h>
#include "U8glib.h"

UltraSonicDistanceSensor hcsr04(12, 11);

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // Name: oled, I2C / TWI
// Display angeschlossen: A4-SDA und A5-SCL

//Schriftgrößen:
//OLED.setFont(u8g_font_unifont); // original
//OLED.setFont(u8g_font_osb21);   // extrem groß "Hello Wor" rest fehlt
//OLED.setFont(u8g_font_helvR14); // Helvetica sehr groß
//OLED.setFont(u8g_font_helvR12); // Helvetica groß
//OLED.setFont(u8g_font_helvR10); // Helvetica relativ groß
//OLED.setFont(u8g_font_helvR08); // Helvetica mittel
//OLED.setFont(u8g_font_04b_03r); // Klein
//OLED.setFont(u8g_font_04b_03br); // Mini


void setup() {
  
}

void loop() {

}