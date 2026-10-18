#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>
#include <LiquidCrystal_I2C.h>
#include "MQ135.h"
#include <SPI.h>
#include "DHT.h"
#include <BH1750.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>

constexpr uint8_t I2C_SDA = 21;
constexpr uint8_t I2C_SCL = 22;

constexpr uint8_t PIN_MQ135 = 34;
constexpr uint8_t PH_PIN = 35;
constexpr uint8_t DHT_PIN = 32;
constexpr uint8_t DHT_TYPE = 11;

constexpr uint8_t CE_PIN = 17;
constexpr uint8_t CSN_PIN = 5;
constexpr uint8_t SCK_PIN = 18;
constexpr uint8_t MISO_PIN = 19;
constexpr uint8_t MOSI_PIN = 23;

constexpr uint8_t RELAY1_PIN = 13;
constexpr uint8_t RELAY2_PIN = 12;
constexpr uint8_t RELAY3_PIN = 14;
constexpr uint8_t RELAY4_PIN = 27;
constexpr uint8_t RELAY5_PIN = 26;
constexpr uint8_t RELAY6_PIN = 25;

template <typename LedType>
inline void setOutputState(uint8_t relayPin, LedType &led, bool enabled)
{
    digitalWrite(relayPin, enabled ? HIGH : LOW);
    if (enabled)
    {
        led.on();
    }
    else
    {
        led.off();
    }
}

template <typename LedType>
inline void driveHysteresisOutput(int sensorValue, int onThreshold, int offThreshold, uint8_t relayPin, LedType &led)
{
    if (sensorValue < onThreshold)
    {
        setOutputState(relayPin, led, true);
    }
    else if (sensorValue > offThreshold)
    {
        setOutputState(relayPin, led, false);
    }
}

inline int wrapIndex(int value, int minValue, int maxValue, bool forward)
{
    if (forward)
    {
        return (value >= maxValue) ? minValue : value + 1;
    }

    return (value <= minValue) ? maxValue : value - 1;
}

inline void adjustPercentValue(int &value, bool increase, int step = 5)
{
    if (increase)
    {
        value += step;
        if (value >= 100)
        {
            value = 0;
        }
    }
    else
    {
        value -= step;
        if (value <= 0)
        {
            value = 100;
        }
    }
}

inline void adjustLuxValue(int &value, bool increase, int step = 40, int maxValue = 80000)
{
    if (increase)
    {
        if (value >= maxValue)
        {
            value = 0;
        }
        else
        {
            value += step;
        }
    }
    else
    {
        if (value <= 0)
        {
            value = maxValue;
        }
        else
        {
            value -= step;
        }
    }
}

inline void adjustPercentPair(int &firstValue, int &secondValue, int selector, bool increase)
{
    if (selector == 0)
    {
        adjustPercentValue(firstValue, increase);
    }
    else
    {
        adjustPercentValue(secondValue, increase);
    }
}

inline void adjustLuxPair(int &firstValue, int &secondValue, int selector, bool increase)
{
    if (selector == 0)
    {
        adjustLuxValue(firstValue, increase);
    }
    else
    {
        adjustLuxValue(secondValue, increase);
    }
}

inline void resetAllRelays()
{
    digitalWrite(RELAY1_PIN, LOW);
    digitalWrite(RELAY2_PIN, LOW);
    digitalWrite(RELAY3_PIN, LOW);
    digitalWrite(RELAY4_PIN, LOW);
    digitalWrite(RELAY5_PIN, LOW);
    digitalWrite(RELAY6_PIN, LOW);
}