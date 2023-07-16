#include "components/ble/OSDService.h"
#include "components/heartrate/HeartRateController.h"
#include "components/ble/NimbleController.h"
#include <nrf_log.h>
#include <math.h>

using namespace Pinetime::Controllers;

namespace {
  // GATT UUIDs
  // 000085xx-0000-1000-8000-00805f9b34fb
  constexpr ble_uuid128_t OSDCharUuid(uint8_t x) {
    return ble_uuid128_t {.u = {.type = BLE_UUID_TYPE_128},
                          .value = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, x, 0x85, 0x00, 0x00}};
  }

  constexpr ble_uuid128_t osdServiceUuid {OSDCharUuid(0xe9)}; // OSD service UUID
  constexpr ble_uuid128_t osdAccCharUuid {OSDCharUuid(0xea)}; // accelerometer data characteristic
}

OSDService::OSDService(NimbleController& nimble, Controllers::MotionController& motionController)
  : nimble {nimble}, motionController {motionController} {
}

void OSDService::OnNewMotionValues() {
}