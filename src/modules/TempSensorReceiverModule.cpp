#include "TempSensorReceiverModule.h"
#include "configuration.h"
#include "nimble/NimbleBluetooth.h"
#include "main.h"

#if TEMPSNSR_RECEIVER

#define BT_HW_ADDR "88:78:73:4a:54:a6"

ProcessMessage TempSensorReceiver::handleReceived(const meshtastic_MeshPacket &mp)
{
    /*
    if(client == nullptr) {
        return ProcessMessage::CONTINUE;
    }

    auto data = (char*)mp.decoded.payload.bytes;
    auto dataSz = mp.decoded.payload.size;
    auto tp = std::atoi(data);
    
    auto srv = client->getServices();
    for(auto i = srv->begin(); i != srv->end(); i++) {
        auto s = *i;
        if(s->getUUID().toString() == "0xd256") {
            auto c = s->getCharacteristics();
            for(auto ch = c->begin(); ch != c->end(); ch++) {
                auto rch = *ch;
                if(rch->getUUID().toString() == "0x0001") {
                    rch->writeValue(data, false);
                    LOG_DEBUG("wrote temp value!");
                    break;
                }
            }
        }
    }

    return ProcessMessage::STOP;
    */

    c->setValue(mp.decoded.payload.bytes, mp.decoded.payload.size);

    return ProcessMessage::STOP;
}

int32_t TempSensorReceiver::runOnce()
{
    /*
    // Try to set up the connection
    if(client == nullptr) {
        if(!NimBLEDevice::getInitialized()) {
            return 100;
        }

        LOG_DEBUG("Setting up the reciever");

        client = NimBLEDevice::createClient();
        
        // search for the target hardware address

        auto scanner = NimBLEDevice::getScan();
        scanner->start(5);
        auto scan = scanner->getResults();

        NimBLEAdvertisedDevice* target = nullptr;

        for(auto i = scan.begin(); i != scan.end(); i++) {
            auto d = *i;
            LOG_DEBUG(d->getAddress().toString().c_str());

            if(d->getAddress().toString() == BT_HW_ADDR) {
                LOG_DEBUG("Found hw device");
                target = d;
                break;
            }
        }

        if(target == nullptr) {
            NimBLEDevice::deleteClient(client);
            client = nullptr;
            return 1000 * 5;
        }

        if(!client->connect(target)) {
            NimBLEDevice::deleteClient(client);
            LOG_ERROR("connection failed! trying again in 5 seconds");
            client = nullptr;
            return 1000 * 5;
        }

        return 1000 * 5;
    }

    LOG_DEBUG("Waiting 50 seconds");
    return 1000 * 50;
    */

    if(srv == nullptr) {

        if(nimbleBluetooth->bleSrv == nullptr) {
            LOG_DEBUG("Not initialized, waiting.");

            return 200;
        }

        srv = nimbleBluetooth->bleSrv;
        auto service = srv->createService("0xd256");
        c = service->createCharacteristic("0x0001", 0x02);

        srv->startAdvertising();
        return 100;
    }

    return 1000 * 50;
}

#endif