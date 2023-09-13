#include <WiFi.h>
#include "Properties.hpp"
#include "MQTTCallbacks.hpp"
#include "Relays.hpp"

const char* TOPIC_PREFIX = "pool/relays";

struct CallbackMapProto {
    const char* postfix;
    void (*callback)(byte* payload, unsigned int length);
};

const int CALLBACKS_COUNT = 2;

void switchRelay1(byte* payload, unsigned int length) {
    uint8_t value = atoi((char*)payload);
    switchRelay1(value == 1 ? true : false);
}

void switchRelay2(byte* payload, unsigned int length) {
    uint8_t value = atoi((char*)payload);
    switchRelay2(value == 1 ? true : false);
}

CallbackMapProto CALLBACKS_PROTO[CALLBACKS_COUNT] = {
    {"0/set", switchRelay1},
    {"1/set", switchRelay2}
};

char checkedTopic[60];

bool topicMatches(char* topic, CallbackMapProto& callbackMap) {
    sprintf(checkedTopic, "%s/%s", TOPIC_PREFIX, callbackMap.postfix);
    return strcmp(topic, checkedTopic) == 0;
}

void callback(char* topic, byte* payload, unsigned int length) {
    char topicCopy[strlen(topic) + 1];
    byte payloadCopy[length];
    strcpy(topicCopy, topic);
    memcpy(payloadCopy, payload, length);
    for (int l = 0; l < CALLBACKS_COUNT; l++) {
        if (topicMatches(topic, CALLBACKS_PROTO[l])) {
            CALLBACKS_PROTO[l].callback(payloadCopy, length);
            break;
        }
    }
}