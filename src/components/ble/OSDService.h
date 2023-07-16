#pragma once
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#include <atomic>
#undef max
#undef min

namespace Pinetime {
  namespace Controllers {
    class NimbleController;
    class MotionController;

    class OSDService {
    public:
      OSDService(NimbleController& nimble, Controllers::MotionController& motionController);
      void Init();
      void OnNewMotionValues();
      void SubscribeNotification(uint16_t attributeHandle);
      void UnsubscribeNotification(uint16_t attributeHandle);

    private:
      NimbleController& nimble;
      Controllers::MotionController& motionController;

      struct ble_gatt_chr_def characteristicDefinition[2];
      struct ble_gatt_svc_def ServiceDefinition[2];

      uint16_t motionValuesHandle;
      std::atomic_bool motionValuesNotificationEnabled {true};
    };
  }
}
