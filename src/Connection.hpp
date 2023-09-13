#pragma once

#include <PubSubClient.h>

void setupWiFi();
void setupMQTT();
void loopMQTT();

extern PubSubClient client;