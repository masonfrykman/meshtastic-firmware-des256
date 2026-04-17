#pragma once

#include "configuration.h"
#include "concurrency/OSThread.h"
#include "mesh/SinglePortModule.h"
#include "MeshService.h"

#include <NimBLEScan.h>
#include <NimBLEDevice.h>
#include <NimBLEClient.h>

class TempSensorConnector: public SinglePortModule, public concurrency::OSThread {
    private:
        bool setupFlag = false;

        NimBLEClient* client = nullptr;

        void fastLog(std::string const& msg) {
            LOG_DEBUG(msg.c_str());
        }

    public:
        TempSensorConnector() : SinglePortModule("!!!DES256 TempSensor!!!", meshtastic_PortNum(256)), concurrency::OSThread("!!DES256!!TempSensor") {}

        virtual int32_t runOnce() override;

};

extern TempSensorConnector* tempSensorMod;

