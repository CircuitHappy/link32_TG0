
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "rom/gpio.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include <stddef.h>
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/timer.h"
#include "protocol_examples_common.h"
#include "gpio.hpp"
#include <ableton/Link.hpp>

using namespace MissingLink;

unsigned int if_nametoindex(const char *ifname) {
    return 0;
}

char *if_indextoname(unsigned int ifindex, char *ifname) {
    return nullptr;
}

struct EngineModel {
  ableton::Link *link;
  GPIO::Pin *clockPin;
};

static intr_handle_t s_timer_handle;

//*****************************************
//*****************************************
//********** TIMER TG0 INTERRUPT **********
//*****************************************
//*****************************************
static void timer_tg0_isr(void* arg)
{
    EngineModel *model = (EngineModel*)arg;

	  //Reset irq and set for next time
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;


    //----- HERE EVERY #uS -----
    int ppqn = 2;
    double quantum = 4.0;
    const auto time = model->link->clock().micros();
    auto sessionState = model->link->captureAudioSessionState();
    const auto beats = sessionState.beatAtTime(time, quantum);
    const double phase = (beats - (int)beats) * ppqn;
    const double clockPhase = (phase - (int)phase);
    // int64_t sTime = esp_timer_get_time();
    model->clockPin->Write(clockPhase < 0.5 ? GPIO::HIGH : GPIO::LOW);
    //int64_t eTime = esp_timer_get_time();
    // std::cout << "Time to run tast: " << (eTime - sTime) << "uSec" << std::endl;
}


//******************************************
//******************************************
//********** TIMER TG0 INITIALISE **********
//******************************************
//******************************************
void timer_tg0_initialise (int timer_period_us, EngineModel *model)
{
    timer_config_t config = {
            .alarm_en = true,				//Alarm Enable
            .counter_en = false,			//If the counter is enabled it will start incrementing / decrementing immediately after calling timer_init()
            .intr_type = TIMER_INTR_LEVEL,	//Is interrupt is triggered on timer’s alarm (timer_intr_mode_t)
            .counter_dir = TIMER_COUNT_UP,	//Does counter increment or decrement (timer_count_dir_t)
            .auto_reload = true,			//If counter should auto_reload a specific initial value on the timer’s alarm, or continue incrementing or decrementing.
            .divider = 80   				//Divisor of the incoming 80 MHz (12.5nS) APB_CLK clock. E.g. 80 = 1uS per timer tick
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_tg0_isr, reinterpret_cast<void*>(model), 0, &s_timer_handle);

    timer_start(TIMER_GROUP_0, TIMER_0);
}


//******************************************
//******************************************
//************ LINK STUFF BELOW ************
//******************************************
//******************************************


void link_task(void* user_param) {
    ableton::Link link(120.0f);
    GPIO::Pin clockPin = GPIO::Pin(GPIO_NUM_18, GPIO::Pin::OUT);
    EngineModel myEngineModel;
    myEngineModel.link = &link;
    myEngineModel.clockPin = &clockPin;
    link.enable(true);

    //HARDWARE TIMER
    timer_tg0_initialise(500, &myEngineModel);

    while (true) {
        vTaskDelay(portMAX_DELAY);
    }
}

extern "C" void app_main() {
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(example_connect());

  xTaskCreatePinnedToCore(
      link_task,
      "link",
      9216,
      nullptr,
      10,
      nullptr, 1);
  vTaskDelete(nullptr);
}
