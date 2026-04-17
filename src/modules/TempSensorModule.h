#pragma once

#include "configuration.h"
#include "concurrency/OSThread.h"

#include <NimBLEScan.h>
#include <NimBLEDevice.h>
#include <NimBLEClient.h>

class TempSensorConnector: public concurrency::OSThread {
    private:
        bool setupFlag = false;

        NimBLEClient* client = nullptr;

        void fastLog(std::string const& msg) {
            LOG_DEBUG(msg.c_str());
        }

    public:
        TempSensorConnector() : concurrency::OSThread("!!DES256!!TempSensor") {}

        virtual int32_t runOnce() override;

};

extern TempSensorConnector* tempSensorMod;

