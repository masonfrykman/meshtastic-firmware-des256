#pragma once

#include "configuration.h"

#if TEMPSNSR_RECEIVER

#include "concurrency/OSThread.h"
#include "mesh/SinglePortModule.h"
#include "MeshService.h"
#include "nimble/NimbleBluetooth.h"

#include <NimBLEDevice.h>

class TempSensorReceiver: public SinglePortModule, public concurrency::OSThread {
    private:
        //NimBLEClient* client = nullptr;
        NimBLEServer* srv = nullptr;
        NimBLECharacteristic* c = nullptr;

    protected:
        virtual ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;

    public:
        TempSensorReceiver(): SinglePortModule("DES256 TempSensorReceiver", meshtastic_PortNum(256)), concurrency::OSThread("!!!DES256!!!TempRecv") {}

        virtual int32_t runOnce() override;
};

#endif