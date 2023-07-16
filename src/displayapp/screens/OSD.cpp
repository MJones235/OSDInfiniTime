#include "displayapp/screens/OSD.h"
#include <lvgl/lvgl.h>
#include <components/heartrate/HeartRateController.h>

#include "displayapp/DisplayApp.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

namespace {
  void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<OSD*>(obj->user_data);
    screen->OnStartStopEvent(event);
  }
}

OSD::OSD(Controllers::MotionController& motionController,
         Controllers::HeartRateController& heartRateController,
         System::SystemTask& systemTask)
  : motionController {motionController}, heartRateController {heartRateController}, systemTask {systemTask} {
  bool isHrRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;

  // print app title
  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "OpenSeizure\nDetector");
  lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);

  // display clock
  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, 0, -20);

  // display current heart rate
  label_hr = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_hr, "Fetching HR...");
  lv_obj_align(label_hr, label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

  // display battery level
  label_battery = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(label_battery, label_hr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  btn_startStop = lv_btn_create(lv_scr_act(), nullptr);
  btn_startStop->user_data = this;
  lv_obj_set_height(btn_startStop, 50);
  lv_obj_set_event_cb(btn_startStop, btnStartStopEventHandler);
  lv_obj_align(btn_startStop, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  label_startStop = lv_label_create(btn_startStop, nullptr);
  UpdateStartStopButton(isHrRunning);
  if (isHrRunning) {
    systemTask.PushMessage(Pinetime::System::Messages::DisableSleeping);
  }

  taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

OSD::~OSD() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
  systemTask.PushMessage(Pinetime::System::Messages::EnableSleeping);
}

void OSD::Refresh() {
  auto state = heartRateController.State();
  switch (state) {
    case Controllers::HeartRateController::States::NoTouch:
    case Controllers::HeartRateController::States::NotEnoughData:
      // case Controllers::HeartRateController::States::Stopped:
      lv_label_set_text_static(label_hr, "000");
      break;
    default:
      lv_label_set_text_fmt(label_hr, "%03d", heartRateController.HeartRate());
  }
}

void OSD::OnStartStopEvent(lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    if (heartRateController.State() == Controllers::HeartRateController::States::Stopped) {
      heartRateController.Start();
      UpdateStartStopButton(heartRateController.State() != Controllers::HeartRateController::States::Stopped);
      systemTask.PushMessage(Pinetime::System::Messages::DisableSleeping);
      lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);
    } else {
      heartRateController.Stop();
      UpdateStartStopButton(heartRateController.State() != Controllers::HeartRateController::States::Stopped);
      systemTask.PushMessage(Pinetime::System::Messages::EnableSleeping);
      lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
    }
  }
}

void OSD::UpdateStartStopButton(bool isRunning) {
  if (isRunning) {
    lv_label_set_text_static(label_startStop, "Stop");
  } else {
    lv_label_set_text_static(label_startStop, "Start");
  }
}
