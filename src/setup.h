#pragma once

#include <esp_adc_cal.h>

#include "sleep.h"

int vref = 1100;

void calibrate_adc()
{
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        (adc_unit_t)ADC_UNIT_1,
        (adc_atten_t)ADC1_CHANNEL_6,
        (adc_bits_width_t)ADC_WIDTH_BIT_12,
        1100,
        &adc_chars);
    // Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        Serial.printf("eFuse Vref:%u mV\n", adc_chars.vref);
        vref = adc_chars.vref;
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    }
    else
    {
        Serial.println("Default Vref: 1100mV");
    }
}

void button_setup()
{
    btn1.setLongClickHandler([](Button2 &b)
                             {
        btnClick = false;
        int r = digitalRead(TFT_BL);
        display.fillScreen(TFT_BLACK);
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.setTextDatum(MC_DATUM);
        display.drawString("Press again to wake up", display.width() / 2, display.height() / 2);
        esp_shallow_sleep(6000);
        digitalWrite(TFT_BL, !r);

        display.writecommand(TFT_DISPOFF);
        display.writecommand(TFT_SLPIN);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start(); });
    btn1.setPressedHandler([](Button2 &b)
                           {
                               // Serial.println("Detect Voltage..");
                               if (TFT_BACKLIGHT_toggle)
                               {
                                   TFT_BACKLIGHT_toggle = false;
                               }
                               else
                               {
                                   TFT_BACKLIGHT_toggle = true;
                               }
                               digitalWrite(TFT_BL, TFT_BACKLIGHT_toggle);
                               // btnCick = true;
                           });

    btn2.setPressedHandler([](Button2 &b)
                           {
        btnClick = false;
        Serial.println("btn press wifi scan"); });
}

void display_setup()
{
    display.init();
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    display.fillScreen(TFT_BLACK);
    display.setTextDatum(MC_DATUM);
    display.setTextSize(2);
    display.setRotation(1);
    display.println("Start...");
}
