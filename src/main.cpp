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
#include <ModbusIP_ESP8266.h>

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

// ModbusIP object
ModbusIP modbus;
// Modbus Registers Offsets
const int MODBUS_SENSOR_IREG = 100;

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

  const char wifi_name[] = "iot";
  const char wifi_password[] = "password";

  Serial.print("\nConnecting to WiFi:\n");
  Serial.print(wifi_name);
  display.print("\nConnecting to WiFi:\n");
  display.println(wifi_name);

  WiFi.begin(wifi_name, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.print(".");
  }

  Serial.print("\nConnected to IP:\n");
  Serial.print(WiFi.localIP());
  display.print("\nConnected - IP:\n");
  display.println(WiFi.localIP());
  delay(2500);

  constexpr int MODBUS_PORT = 502; // Modbus TCP port
  modbus.server(502);              // Start Modbus IP
  // Add MODBUS_SENSOR_IREG register - Use addIreg() for analog Inputs
  modbus.addIreg(MODBUS_SENSOR_IREG);
}

#pragma endregion Setup

// Function prototypes
float get_distance_cm_from_ultrasound_sensor();
void button_loop();
float check_max_change(float previousReading, float currentReading, float maxAllowedChange);

uint16_t lastDistanceCmUint16 = 0;         // Variable to store distance in cm
uint16_t maximalChangeBetweenReadings = 5; // Maximal change between readings

void loop()
{
  // Call once inside loop() - all magic here
  modbus.task();

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

  // Convert distanceCm from float to uint16_t
  uint16_t distanceCmUint16 = static_cast<uint16_t>(distanceCm);

  // Check the maximal change between readings
  distanceCmUint16 = static_cast<uint16_t>(check_max_change(lastDistanceCmUint16, distanceCmUint16, maximalChangeBetweenReadings));
  lastDistanceCmUint16 = distanceCmUint16; // Update last distance

  // Update Modbus register with the converted value
  modbus.Ireg(MODBUS_SENSOR_IREG, distanceCmUint16);

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
  delayMicroseconds(5);

  // Trigger ultrasonic pulse
  digitalWrite(ULTRASOUND_TRIGER_PIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(ULTRASOUND_TRIGER_PIN, LOW);

  // Measure pulse duration
  long duration = pulseIn(ULTRASOUND_ECHO_PIN, HIGH);

  // Calculate distance in cm
  return (duration * SOUND_SPEED) / 2;
}

// Function to check the maximal change between readings and return the corresponding value
float check_max_change(float previousReading, float currentReading, float maxAllowedChange)
{
  float change = abs(currentReading - previousReading);
  if (change > maxAllowedChange)
  {
    return previousReading; // Return the previous reading if the change exceeds the allowed maximum
  }
  return currentReading; // Return the current reading if the change is within the allowed maximum
}
