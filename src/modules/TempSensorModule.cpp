#include "TempSensorModule.h"

#define BLUEFRUIT_HW_ADDR "d1:f0:8a:18:1b:c2"

int32_t TempSensorConnector::runOnce() {
    if(!setupFlag) {
        // wait until NimBLEDevice is initialized
        if(!NimBLEDevice::getInitialized()) {
            return 100;
        }

        NimBLEScan* scanner = NimBLEDevice::getScan();
        NimBLEScanResults res = scanner->start(5);
        
        NimBLEAdvertisedDevice* target = nullptr;

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

        setupFlag = true;

        // Connect to the found device
        client = NimBLEDevice::createClient();
        if(!client->connect(target)) {
            LOG_ERROR("Failed to connect to our Bluefruit device! Trying again in 10 seconds");
            setupFlag = false;
            return 1000 * 10;
        }

        // Go through the services advertised by the device and look for our temperature service
        
    } else {
        // We're already setup, so refresh the temperature characteristic
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
            // TODO: handle error
            client->disconnect();
            setupFlag = false;
            return 1000 * 10;
        }
        
        auto chars = tempService->getCharacteristics(true);
        std::string val;
        for(auto i = chars->begin(); i != chars->end(); i++) {
            auto x = *i;
            fastLog("Characteristic (selected service): " + x->getUUID().toString());
            fastLog("Value: " + std::to_string(x->readValue<uint16_t>()));
            val = std::to_string(x->readValue<uint16_t>());
        }

        auto dp = this->allocDataPacket();
        dp->channel = 1;
        dp->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
        // TODO: need to get the data somehow

        for(size_t i = 0; i <= val.length(); i++) {
            dp->decoded.payload.bytes[i] = val.c_str()[i];
        }
        dp->decoded.payload.size = strlen(val.c_str());

        service->sendToMesh(dp, RX_SRC_LOCAL, true);
        return 1000 * 15;
    }

    return 1000 * 10;
}
