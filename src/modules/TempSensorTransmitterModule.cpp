#include "TempSensorTransmitterModule.h"

#if TEMPSNSR_TRANSMITTER

#define BLUEFRUIT_HW_ADDR "d1:f0:8a:18:1b:c2"
#define SEND_INTERVAL_SECS 15
#define FRIDGE_ID "10"

#define TEMPSNSR_TRANSMITTER_MOCK false

int32_t TempSensorTransmitter::runOnce() {
#if !TEMPSNSR_TRANSMITTER_MOCK
    if(client == nullptr) {
        // wait until NimBLEDevice is initialized
        if(!NimBLEDevice::getInitialized()) {
            return 100;
        }

        NimBLEScan* scanner = NimBLEDevice::getScan();
        NimBLEScanResults res = scanner->start(5);
        
        NimBLEAdvertisedDevice* target = nullptr;

        // Search for the hardware address of our device
        for(auto i = res.begin(); i != res.end(); i++) {
            auto x = *i;
            fastLog("Found BLE Device in scan: " + x->getAddress().toString());
            if(x->getAddress().toString() == BLUEFRUIT_HW_ADDR) {
                // found the hardware address of our bluefruit
                fastLog("Found our Bluefruit device!");
                target = x;
                break;
            }
        }

        if(target == nullptr) {
            // Didn't find the device we want, go ahead and scan again in 10 seconds.
            return 1000 * 10;
        }

        // Connect to the found device
        client = NimBLEDevice::createClient();
        if(!client->connect(target)) {
            LOG_ERROR("Failed to connect to our Bluefruit device! Trying again in 10 seconds");
            NimBLEDevice::deleteClient(client);
            client = nullptr;
            return 1000 * 10;
        }

        // Go through the services advertised by the device and look for our temperature service
        return 1000;
    } else {

        // Make sure we're still connected
        if(!client->isConnected()) {
            fastLog("Client disconnected, trying to reconnect.");
            NimBLEDevice::deleteClient(client);
            client = nullptr;
            return 100;
        }

        // We're already setup, so refresh the temperature service & characteristic
        NimBLERemoteService* tempService;

        auto services = client->getServices(true);
        for(auto i = services->begin(); i != services->end(); i++) {
            NimBLERemoteService* service = *i;
            fastLog("Service: " + service->toString());
            if(service->getUUID().toString() == "0xd256") {
                tempService = service;
                fastLog("Found our service!");
            }
        }

        if(tempService == nullptr) {
            LOG_ERROR("The connected device doesn't have a valid 0xd256 temperature service, disconnecting and trying again in 10 seconds.");
            client->disconnect();
            NimBLEDevice::deleteClient(client);
            client = nullptr;
            return 1000 * 10;
        }
        
        // Read the temperature reading published by the temperature sensor Arduino
        auto chars = tempService->getCharacteristics(true);
        std::string reading;
        for(auto i = chars->begin(); i != chars->end(); i++) {
            auto x = *i;
            fastLog("Characteristic (selected service): " + x->getUUID().toString());
            fastLog("Value: " + std::to_string(x->readValue<uint16_t>()));
            reading = std::to_string(x->readValue<uint16_t>());
        }

        // Assemble into valid message.
        std::string val = "TSNSR_";
        val.append(FRIDGE_ID);
        val.append("_");
        val.append(reading);
#else
        // Use mock data
        // "TSNSR_<FRIDGE ID>_<TEMPERATURE>"
        // The TSNSR is needed as the Meshtastic Python API will combine every portnum 256-511 under the PRIVATE_APP enum, so this is needed to differentiate it from another potential private app.
        std::string val = "TSNSR_14_35";
#endif // !TEMPSNSR_TRANSMITTER_MOCK
        auto dp = this->allocDataPacket();
        dp->channel = 1;

        for(size_t i = 0; i <= val.length(); i++) {
            dp->decoded.payload.bytes[i] = val.c_str()[i];
        }
        dp->decoded.payload.size = strlen(val.c_str());

        service->sendToMesh(dp, RX_SRC_LOCAL, true);

        // wait another SEND_INTERVAL_SECS before refreshing
        return 1000 * SEND_INTERVAL_SECS;

#if !TEMPSNSR_TRANSMITTER_MOCK
    }


    return 1000 * 10;
#endif
}

#endif // TEMPSNSR_TRANSMITTER