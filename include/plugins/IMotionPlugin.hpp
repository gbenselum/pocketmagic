#pragma once

#include <cstdint>
#include <cstddef>

namespace Plugins {

/**
 * @brief Abstract interface for Motion Processors (IMU algorithms).
 */
class IMotionPlugin {
public:
    virtual ~IMotionPlugin() = default;

    /**
     * @brief Human-readable name of the motion algorithm.
     */
    virtual const char* getName() const = 0;

    /**
     * @brief Initialize plugin state, buffers, and filters.
     */
    virtual void init() = 0;

    /**
     * @brief Process 6-axis IMU readings.
     * @param pitch Calculated pitch angle in degrees (headstock tilt).
     * @param roll Calculated roll angle in degrees.
     * @param accel Raw or calibrated accelerometer [x, y, z] in Gs.
     * @param gyro Raw or calibrated gyroscope [x, y, z] in deg/s.
     */
    virtual void processIMU(float pitch, float roll, const float accel[3], const float gyro[3]) = 0;

    /**
     * @brief Query whether the plugin has a pending parameter command to transmit.
     * @param outModuleId Destination effect module ID (0..9).
     * @param outParamId Target parameter index within the module.
     * @param outValue Normalized target parameter value (0.0f - 1.0f or 0..100).
     * @return true if a command is ready to be sent, false otherwise.
     */
    virtual bool getTargetCommand(uint8_t& outModuleId, uint8_t& outParamId, float& outValue) = 0;

    /**
     * @brief Reset internal filters, accumulators, and state.
     */
    virtual void reset() = 0;
};

} // namespace Plugins
