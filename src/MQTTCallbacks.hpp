#pragma once

#include <WiFi.h>

void callback(char *topic, byte *payload, unsigned int length);
void setupMQTTCallbacks();
