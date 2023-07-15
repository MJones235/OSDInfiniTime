#include "components/ble/OSDService.h"
#include "components/heartrate/HeartRateController.h"
#include "components/ble/NimbleController.h"
#include <nrf_log.h>

using namespace Pinetime::Controllers;

OSDService::OSDService(NimbleController& nimble) : nimble {nimble} {
}
