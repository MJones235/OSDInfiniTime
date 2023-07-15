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

    class OSDService {
    public:
      OSDService(NimbleController& nimble);

    private:
      NimbleController& nimble;
    };
  }
}
