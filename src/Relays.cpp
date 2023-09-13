#include <Arduino.h>
#include "Connection.hpp"
#include "Display.hpp"

const unsigned long NEXT_REPORT_PERIOD = 60 * 1000UL;
const uint8_t RELAYS_COUNT = 2;
const char* STATE_TOPIC_TMPL = "pool/relays/%d";

struct Relay {
    u_int8_t pin;
    boolean state;
    boolean locked;
};

Relay relays[RELAYS_COUNT] = {
    {26, false, false}, {27, false, false}
};

void report(int relay, boolean mode) {
    char topic[50];
    sprintf(topic, STATE_TOPIC_TMPL, relay);
    client.publish(topic, mode ? "1" : "0");
}

void report(int relay) {
    report(relay, relays[relay].state);
}

void changeStateAndReport(int relay, boolean mode) {
    relays[relay].state = mode;
    report(relay, mode);
}

void switchRelay(int relay, boolean mode) {
    if (relays[relay].locked) {
        report(relay);
    }
    else {
        digitalWrite(relays[relay].pin, mode ? LOW : HIGH);
        changeStateAndReport(relay, mode);
        displayRelays[relay].state = mode;
        displayRelays[relay].lastChange = millis();
    }
}

void setupRelays() {
    for (u_int8_t l = 0; l < RELAYS_COUNT; l++) {
        pinMode(relays[l].pin, OUTPUT);
    }
    for (u_int8_t l = 0; l < RELAYS_COUNT; l++) {
        switchRelay(l, false);
    }
}

void switchRelay1(boolean mode) {
    switchRelay(0, mode);
}

void switchRelay2(boolean mode) {
    switchRelay(1, mode);
}

void lockRelay(int relay, boolean mode, boolean lock) {
    digitalWrite(relays[relay].pin, mode ? HIGH : LOW);
    relays[relay].locked = lock;
    changeStateAndReport(relay, mode);
}

void lockRelay1(boolean mode, boolean lock) {
    lockRelay(0, mode, lock);
}

void lockRelay2(boolean mode, boolean lock) {
    lockRelay(1, mode, lock);
}

void handleRelays() {
    static unsigned long lastSwitchTime = 0;
    unsigned long thisLoopTime = millis();
    if (thisLoopTime - lastSwitchTime < NEXT_REPORT_PERIOD) {
        return;
    }
    lastSwitchTime = thisLoopTime;
    for (u_int8_t l = 0; l < RELAYS_COUNT; l++) {
        report(l);
    }
}
