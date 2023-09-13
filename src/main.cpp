#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "Properties.hpp"
#include "Connection.hpp"
#include "BluetoothSensor.hpp"
#include "DS18b20.hpp"
#include "Relays.hpp"
#include "Controls.hpp"
#include "Display.hpp"

void setup() {
  setupWiFi();
  setupMQTT();

  ArduinoOTA
    .onStart([]() {})
    .onEnd([]() {})
    .onProgress([](unsigned int progress, unsigned int total) {})
    .onError([](ota_error_t error) {});
  ArduinoOTA.begin();

  bluetoothSetup();
  setupDS18b20();
  setupRelays();
  setupDisplay();
  // setupControls();
}

void loop() {
  loopMQTT();
  ArduinoOTA.handle();
  bluetoothHandle();
  handleDS18b20();
  handleRelays();
  handleDisplay();
  // handleControls();
  delay(1000);
}