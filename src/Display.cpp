#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Display.hpp"

const unsigned long NEXT_REDRAW_PERIOD = 15 * 1000UL;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

const char* ON = "ON";
const char* OFF = "OFF";

#define SCREEN_COUNT 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DisplayWaterQuality waterQuality = { 0,0,0,0,0,0,0 };
char buffer[200];

void displayWaterQuality() {
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    sprintf(buffer, "TMP: %.1f PH: %.1f", waterQuality.temp, waterQuality.ph);
    display.println(buffer);
    sprintf(buffer, "CHLR: %.1f ORP: %d", waterQuality.cloro, waterQuality.orp);
    display.println(buffer);
    sprintf(buffer, "EC: %d TDS: %d", waterQuality.ec, waterQuality.tds);
    display.println(buffer);
    sprintf(buffer, "BATT: %.2f", waterQuality.battery);
    display.print(buffer);
}

DisplayRelay displayRelays[] = {
    {false, 0},
    {false, 0}
};

const unsigned long MINUTE_IN_MILLIS = 60 * 1000;
const unsigned long HOURS_IN_MILLIS = 60 * MINUTE_IN_MILLIS;
const unsigned long DAYS_IN_MILLIS = 24 * HOURS_IN_MILLIS;

void convertToPeriod(unsigned long periodInMillis, char* buffer) {
    unsigned long p = periodInMillis;
    unsigned long days = p / DAYS_IN_MILLIS;
    p = p % DAYS_IN_MILLIS;
    unsigned long hours = p / HOURS_IN_MILLIS;
    p = p % HOURS_IN_MILLIS;
    unsigned long minutes = p / MINUTE_IN_MILLIS;
    sprintf(buffer, "%dd %dh %dm", days, hours, minutes);
}

void displayRelaysState() {
    auto time = millis();
    display.setTextColor(WHITE);
    display.setCursor(10, 12);
    display.setTextSize(2);
    display.print(displayRelays[0].state ? ON : OFF);
    display.setCursor(10, 40);
    display.setTextSize(1);
    convertToPeriod(time - displayRelays[0].lastChange, buffer);
    display.print(buffer);
    display.setCursor(74, 12);
    display.setTextSize(2);
    display.print(displayRelays[1].state ? ON : OFF);
    display.setCursor(74, 40);
    display.setTextSize(1);
    convertToPeriod(time - displayRelays[1].lastChange, buffer);
    display.print(buffer);
}

void (*displayScreen[])() = {
    displayWaterQuality,
    displayRelaysState
};

void setupDisplay() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
}

void handleDisplay() {
    static u_int8_t screen = 0;
    static unsigned long lastSwitchTime = 0;
    unsigned long thisLoopTime = millis();
    if (thisLoopTime - lastSwitchTime < NEXT_REDRAW_PERIOD) {
        return;
    }
    screen = (screen + 1) % SCREEN_COUNT;
    lastSwitchTime = thisLoopTime;
    display.clearDisplay();
    displayScreen[screen]();
    display.display();
}
