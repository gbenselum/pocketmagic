#pragma once

#include <cstdint>
#include <cstddef>

namespace Config {

// Device Information
constexpr const char* DEVICE_NAME = "SonicMotionESP32";
constexpr const char* FIRMWARE_VERSION = "0.1.0-alpha";

// BLE GATT Configuration
constexpr const char* TARGET_BLE_DEVICE_NAME = "Sonic Master BLE";
constexpr const char* TARGET_SERVICE_UUID = "03b80e5a-ede8-4b33-a751-6ce34ec4c700";
constexpr const char* TARGET_CHAR_UUID    = "7772e5db-3868-4112-a1a9-f2669d106bf3";

// Rate Limiting & Safety
constexpr uint32_t MAX_BLE_WRITES_PER_SEC = 20;
constexpr uint32_t BLE_WRITE_INTERVAL_MS  = 1000 / MAX_BLE_WRITES_PER_SEC; // 50ms
constexpr size_t   MAX_SYSEX_PAYLOAD_SIZE = 256;

// IMU Sensor Configuration (MPU6886)
constexpr float IMU_SAMPLE_RATE_HZ   = 100.0f;
constexpr uint32_t IMU_SAMPLE_PERIOD_MS = 10; // 100Hz = 10ms
constexpr float PITCH_MIN_DEG        = 10.0f;
constexpr float PITCH_MAX_DEG        = 50.0f;
constexpr float PITCH_EMA_ALPHA      = 0.2f;  // Exponential moving average filter coefficient

// UI Configuration
constexpr int SCREEN_WIDTH           = 320;
constexpr int SCREEN_HEIGHT          = 240;
constexpr int SWIPE_THRESHOLD_X      = 60;   // px horizontal movement required for swipe
constexpr int SWIPE_MAX_DEVIATION_Y  = 35;   // px max vertical tolerance during swipe
constexpr uint32_t SWIPE_MAX_TIME_MS = 300;  // ms max gesture duration

// FreeRTOS Task Configuration
constexpr uint32_t STACK_SIZE_BLE_TASK      = 4096;
constexpr uint32_t STACK_SIZE_PROTOCOL_TASK = 4096;
constexpr uint32_t STACK_SIZE_IMU_TASK      = 4096;
constexpr uint32_t STACK_SIZE_GUI_TASK      = 8192;

constexpr uint8_t PRIORITY_BLE_TASK      = 3;
constexpr uint8_t PRIORITY_PROTOCOL_TASK = 3;
constexpr uint8_t PRIORITY_IMU_TASK      = 4; // High priority for smooth motion sampling
constexpr uint8_t PRIORITY_GUI_TASK      = 2;

constexpr int CORE_COMMS = 0;
constexpr int CORE_APP   = 1;

// FreeRTOS Queue Capacities
constexpr size_t BLE_WRITE_QUEUE_LEN  = 16;
constexpr size_t MOTION_EVENT_QUEUE_LEN = 16;

} // namespace Config
