#include <OneWire.h>
#include <DallasTemperature.h>
#include "Connection.hpp"

const int ONE_WIRE_PIN = 4;
const char* TOPIC_TMPL = "pool/ds18b20/%x%x%x%x%x%x%x%x/temp";
const unsigned long NEXT_READ_PERIOD = 60 * 1000UL;

OneWire oneWire(ONE_WIRE_PIN);

DeviceAddress dA;

DallasTemperature sensors(&oneWire);

void setupDS18b20() {
    sensors.begin();
}

void handleDS18b20() {
    static unsigned long lastReadTime = 0;
    char tmpValueStrBuffer[8];
    char topicBuffer[50];
    unsigned long thisLoopTime = millis();
    if (thisLoopTime - lastReadTime < NEXT_READ_PERIOD) {
        return;
    }
    uint8_t count = sensors.getDS18Count();
    lastReadTime = thisLoopTime;
    sensors.requestTemperatures();
    for (uint8_t i = 0; i < count; i++) {
        float tmpValue = sensors.getTempCByIndex(i);
        sensors.getAddress(dA, i);
        dtostrf(tmpValue, 1, 2, tmpValueStrBuffer);
        sprintf(topicBuffer, TOPIC_TMPL, dA[0],dA[1],dA[2],dA[3],dA[4],dA[5],dA[6],dA[7]);
        client.publish(topicBuffer, tmpValueStrBuffer);
    }
}
