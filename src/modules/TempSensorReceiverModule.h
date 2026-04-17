#pragma once

#include "configuration.h"

#if TEMPSNSR_RECEIVER

#include "concurrency/OSThread.h"
#include "mesh/SinglePortModule.h"
#include "MeshService.h"

class TempSensorReceiver: public SinglePortModule, public concurrency::OSThread {
    public:
        TempSensorReceiver(): SinglePortModule("DES256 TempSensorReceiver", meshtastic_PortNum(256)), concurrency::OSThread("!!!DES256!!!TempRecv") {}

        virtual int32_t runOnce() override;
};

#endif