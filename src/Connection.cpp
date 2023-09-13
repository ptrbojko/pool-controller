#include <WiFi.h>
#include "Properties.hpp"
#include "Connection.hpp"
#include "MQTTCallbacks.hpp"

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
}

void setupMQTT() {
    //connecting to a mqtt broker
    client.setServer(mqttBroker, mqttPort);
    client.setCallback(callback);
}

void loopMQTT() {
    while (!client.connected()) {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        if (client.connect(client_id.c_str(), mqttUsername, mqttPassword)) {
            client.subscribe("pool/relays/+/set");
        }
        else {
            delay(2000);
        }
    }
     if (client.connected()) {
        client.loop();
        return;
    }
}

