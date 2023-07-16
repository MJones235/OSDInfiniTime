#pragma once

#include <cstdint>
#include <chrono>
#include "displayapp/screens/Screen.h"
#include "systemtask/SystemTask.h"
#include <lvgl/src/lv_core/lv_style.h>
#include <lvgl/src/lv_core/lv_obj.h>
#include "utility/DirtyValue.h"

namespace Pinetime {
  namespace Controllers {
    class HeartRateController;
  }

  namespace Applications {
    namespace Screens {

      class OSD : public Screen {
      public:
        OSD(Controllers::MotionController& motionController,
            Controllers::HeartRateController& HeartRateController,
            System::SystemTask& systemTask);
        ~OSD() override;

        void Refresh() override;

        void OnStartStopEvent(lv_event_t event);

      private:
        Controllers::MotionController& motionController;
        Controllers::HeartRateController& heartRateController;
        Pinetime::System::SystemTask& systemTask;
        void UpdateStartStopButton(bool isRunning);
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>> time {};
        lv_obj_t* label_hr;
        lv_obj_t* label_time;
        lv_obj_t* label_battery;
        lv_obj_t* btn_startStop;
        lv_obj_t* label_startStop;

        lv_task_t* taskRefresh;
      };
    }
  }
}
