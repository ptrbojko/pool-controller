#include <Arduino.h>
#include "Relays.hpp"

const u_int8_t CONTROL_1 = 14;
const u_int8_t CONTROL_2 = 15;

struct Control {
    boolean state;
    void (*callback)(boolean mode, boolean lock);
};

Control controls[2] = {
    {false, lockRelay1},
    {false, lockRelay2}
};

void setupControls() {
    pinMode(CONTROL_1, INPUT);
    pinMode(CONTROL_2, INPUT);
}

void handleCtrl(u_int8_t ctrl, int state, boolean relayState) {
    boolean s = state == HIGH;
    if (controls[ctrl].state != s) {
        controls[ctrl].state = s;
        controls[ctrl].callback(s ? relayState : !relayState, s);
    }
}


void handleControls() {
    handleCtrl(0, digitalRead(CONTROL_1), true);
    handleCtrl(0, digitalRead(CONTROL_2), false);
}