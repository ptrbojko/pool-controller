#include <NimBLEDevice.h>
#include "BluetoothSensor.hpp"
#include "Connection.hpp"
#include "Display.hpp"

#define DESIRED_MESSAGE_SIZE 29

const unsigned long NEXT_READ_PERIOD = 10 * 60 * 1000UL;
const NimBLEUUID SERVICE_UUID((uint16_t)0xFF01);
const NimBLEUUID CHAR_ID((uint16_t)0xFF02);

NimBLEAdvertisedDevice* poolSensorDevice = nullptr;
NimBLEScan* scan = nullptr;
NimBLEClient* poolClient = nullptr;

class ScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(SERVICE_UUID)) {
            poolSensorDevice = advertisedDevice;
            client.publish("pool/sensor/status", "FOUND");
        }
        else {
            client.publish("pool/bluetooth", advertisedDevice->getName().c_str());
        }
    }
};

void bluetoothSetup() {
    client.publish("pool/sensor/status", "start");
    NimBLEDevice::init("");
    scan = NimBLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new ScanCallbacks(), false);
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
};

void publishFloat(const char* topic, float value) {
    char buffer[8];
    dtostrf(value, 1, 2, buffer);
    client.publish(topic, buffer);
};

void publishInt(const char* topic, int value) {
    char buffer[8];
    ltoa(value, buffer, 10);
    client.publish(topic, buffer);
};

void decode(const uint8_t* data, int size) {
    if (size < DESIRED_MESSAGE_SIZE) {
        return;
    }
    uint8_t tmp = 0;
    uint8_t hibit = 0;
    uint8_t lobit = 0;
    uint8_t hibit1 = 0;
    uint8_t lobit1 = 0;
    uint8_t* message = (uint8_t*)data;

    for (int i = size - 1; i > 0; i--) {
        tmp = message[i];
        hibit1 = (tmp & 0x55) << 1;
        lobit1 = (tmp & 0xAA) >> 1;
        tmp = message[i - 1];
        hibit = (tmp & 0x55) << 1;
        lobit = (tmp & 0xAA) >> 1;
        message[i] = ~(hibit1 | lobit);
        message[i - 1] = ~(hibit | lobit1);
    }

    // (()-1400)/(3100-1400)

    waterQuality.temp = ((message[13] << 8) + message[14]) / 10.0;
    waterQuality.ph = ((message[3] << 8) + message[4]) / 100.0;
    waterQuality.orp = ((message[20] << 8) + message[21]);
    waterQuality.battery = 100.0 * (((message[15] << 8) + message[16]) - 1400.0) / (3100.0 - 1400.0);
    waterQuality.ec = ((message[5] << 8) + message[6]);
    waterQuality.tds = ((message[7] << 8) + message[8]);
    waterQuality.cloro = ((message[11] << 8) + message[12]) / 10.0;

    publishFloat("pool/sensor/temp", waterQuality.temp);
    publishFloat("pool/sensor/ph", waterQuality.ph);
    publishInt("pool/sensor/orp", waterQuality.orp);
    publishFloat("pool/sensor/battery", waterQuality.battery);
    publishInt("pool/sensor/ec", waterQuality.ec);
    publishInt("pool/sensor/tds", waterQuality.tds);
    publishFloat("pool/sensor/cloro", waterQuality.cloro);

};

void bluetoothHandle() {
    static unsigned long lastReadTime = 0;
    unsigned long thisLoopTime = millis();
    if (poolSensorDevice == nullptr && !scan->isScanning()) {
        scan->start(15, nullptr, false);
        client.publish("pool/sensor/status", "SCANNING");
    }
    else if (poolSensorDevice != nullptr) {
        if (scan->isScanning()) {
            scan->stop();
        }
        if (poolClient == nullptr) {
            poolClient = NimBLEDevice::createClient();
            poolClient->setConnectTimeout(7);
        }
        if (lastReadTime != 0 && (thisLoopTime - lastReadTime < NEXT_READ_PERIOD)) {
            return;
        }
        client.publish("pool/sensor/status", "BEFORE_FETCH");
        if (!poolClient->isConnected() && poolClient->connect(poolSensorDevice)) {
            client.publish("pool/sensor/status", "CONNECTED");
            NimBLERemoteService* service = poolClient->getService(SERVICE_UUID);
            if (service == nullptr) {
                client.publish("pool/sensor/status", "ERR_SRV_NULL");
                return;
            }
            NimBLERemoteCharacteristic* characteristic = service->getCharacteristic(CHAR_ID);
            if (characteristic == nullptr) {
                client.publish("pool/sensor/status", "ERR_CHR_NULL");
                return;
            }
            NimBLEAttValue value = characteristic->readValue();
            decode(value.data(), value.size());
            poolClient->disconnect();
            lastReadTime = thisLoopTime;
        }
        else {
            client.publish("pool/sensor/status", "ERR_CONNECT");
        }
        client.publish("pool/sensor/status", "AFTER_FETCH");
    }
};
