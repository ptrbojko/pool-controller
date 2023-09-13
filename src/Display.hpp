#pragma once

struct DisplayWaterQuality {
    float temp;
    float ph;
    int orp;
    float battery;
    int ec;
    int tds;
    float cloro;
};

extern DisplayWaterQuality waterQuality;

struct DisplayRelay {
    bool state;
    unsigned long lastChange;
};

extern DisplayRelay displayRelays[];

void setupDisplay();
void handleDisplay();