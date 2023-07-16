#include "components/ble/BatteryInformationService.h"
#include <nrf_log.h>
#include "components/battery/BatteryController.h"

using namespace Pinetime::Controllers;

namespace {
  // GATT UUIDs
  // 000085xx-0000-1000-8000-00805f9b34fb
  constexpr ble_uuid128_t OSDCharUuid(uint8_t x) {
    return ble_uuid128_t {.u = {.type = BLE_UUID_TYPE_128},
                          .value = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, x, 0x85, 0x00, 0x00}};
  }

  constexpr ble_uuid128_t osdServiceUuid {OSDCharUuid(0xe9)};  // OSD service UUID
  constexpr ble_uuid128_t osdBattCharUuid {OSDCharUuid(0xeb)}; // battery level characteristic
}

constexpr ble_uuid16_t BatteryInformationService::batteryInformationServiceUuid;
constexpr ble_uuid16_t BatteryInformationService::batteryLevelUuid;

int BatteryInformationServiceCallback(uint16_t /*conn_handle*/, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto* batteryInformationService = static_cast<BatteryInformationService*>(arg);
  return batteryInformationService->OnBatteryServiceRequested(attr_handle, ctxt);
}

BatteryInformationService::BatteryInformationService(Controllers::Battery& batteryController)
  : batteryController {batteryController},
    characteristicDefinition {{.uuid = &batteryLevelUuid.u,
                               .access_cb = BatteryInformationServiceCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                               .val_handle = &batteryLevelHandle},
                              {0}},
    osdCharacteristicDefinition {{.uuid = &osdBattCharUuid.u,
                                  .access_cb = BatteryInformationServiceCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                                  .val_handle = &osdBatteryLevelHandle},
                                 {0}},
    serviceDefinition {
      {/* Battery Information Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &batteryInformationServiceUuid.u,
       .characteristics = characteristicDefinition},
      {/* OSD Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &osdServiceUuid.u,
       .characteristics = osdCharacteristicDefinition},
      {0},
    } {
}

void BatteryInformationService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int BatteryInformationService::OnBatteryServiceRequested(uint16_t attributeHandle, ble_gatt_access_ctxt* context) {
  if (attributeHandle == batteryLevelHandle || attributeHandle == osdBatteryLevelHandle) {
    NRF_LOG_INFO("BATTERY : handle = %d", attributeHandle);
    uint8_t batteryValue = batteryController.PercentRemaining();
    int res = os_mbuf_append(context->om, &batteryValue, 1);
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}

void BatteryInformationService::NotifyBatteryLevel(uint16_t connectionHandle, uint8_t level) {
  auto* om = ble_hs_mbuf_from_flat(&level, 1);
  ble_gattc_notify_custom(connectionHandle, batteryLevelHandle, om);
  ble_gattc_notify_custom(connectionHandle, osdBatteryLevelHandle, om);
}
