/*
  Application to monitor the water level in the water tank.
  The water level is measured using ultrasonic sensor HC-SR04.
  The data is displayed on the TTGO T-Display ESP32.

  Created by JarekParal (github.com/jarekparal)
  Code for TTGO Display was copy from https://github.com/Xinyuan-LilyGO/TTGO-T-Display.

  License: MIT
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#pragma region Setup

constexpr int BUTTON_1_PIN = 35;
constexpr int BUTTON_2_PIN = 0;
Button2 btn1(BUTTON_1_PIN);
Button2 btn2(BUTTON_2_PIN);
bool btnClick = false;

constexpr int ULTRASOUND_TRIGER_PIN = 26;
constexpr int ULTRASOUND_ECHO_PIN = 27;

TFT_eSPI display = TFT_eSPI(135, 240); // Initialize TFT display
bool TFT_BACKLIGHT_toggle = TFT_BACKLIGHT_ON;

constexpr int ONE_WIRE_BUS_PIN = 13;       // Pin for DS18B20 sensor
OneWire oneWire(ONE_WIRE_BUS_PIN);         // Setup a oneWire instance
DallasTemperature dallasSensors(&oneWire); // Pass oneWire reference to Dallas Temperature

#include "setup.h"

void setup()
{
  // Configure pins for ultrasonic sensor
  pinMode(ULTRASOUND_TRIGER_PIN, OUTPUT);
  pinMode(ULTRASOUND_ECHO_PIN, INPUT);

  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Start");

  // Initialize buttons and TFT display
  button_setup();
  display_setup();
}

#pragma endregion Setup

// Function prototypes
float get_distance_cm_from_ultrasound_sensor();
void button_loop();

void loop()
{
  button_loop();

  const float distanceCm = get_distance_cm_from_ultrasound_sensor();

  // Print distance to Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  // Display distance on TFT screen
  display.fillScreen(TFT_BLACK);
  display.setCursor(0, 0);
  display.print("Distance (cm): ");
  display.println(distanceCm);

  delay(1000); // Wait for 1 second
}

void button_loop()
{
  // Process button events
  btn1.loop();
  btn2.loop();
}

float get_distance_cm_from_ultrasound_sensor()
{
  // Define sound speed in cm/uS
  constexpr float SOUND_SPEED = 0.034;

  // Clear the trigPin
  digitalWrite(ULTRASOUND_TRIGER_PIN, LOW);
  delayMicroseconds(2);

  // Trigger ultrasonic pulse
  digitalWrite(ULTRASOUND_TRIGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASOUND_TRIGER_PIN, LOW);

  // Measure pulse duration
  long duration = pulseIn(ULTRASOUND_ECHO_PIN, HIGH);

  // Calculate distance in cm
  return (duration * SOUND_SPEED) / 2;
}
