#pragma once

#include <Arduino.h>

void esp_shallow_sleep(int ms)
{
    // Enable the timer wakeup for the specified duration in microseconds
    esp_sleep_enable_timer_wakeup(ms * 1000);

    // Keep the RTC peripherals powered on during sleep
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // Start light sleep mode
    esp_light_sleep_start();
}
