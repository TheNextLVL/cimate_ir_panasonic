#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace climate_ir_panasonic {

// Temperature
const uint8_t TEMP_MIN = 16;  // Celsius
const uint8_t TEMP_MAX = 30;  // Celsius
const uint8_t TEMP_OFFSET = 15;  // Temperature offset to get same bits as in ir code

const uint16_t CARRIER_FRECUENCY = 38000;  // In Hz
const uint16_t MARK_LEAD_INOUT = 3487;
const uint16_t BIT_MARK = 800;
const uint16_t ZERO_SPACE = 920;
const uint16_t ONE_SPACE = 2650;

const uint8_t STATE_FRAME_SIZE = 4;

class PanasonicClimate : public climate_ir::ClimateIR {
    public:
        PanasonicClimate()
            : climate_ir::ClimateIR(TEMP_MIN, TEMP_MAX, 1.0f, true, false,
                                    {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                                    {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL},
                                    {climate::CLIMATE_PRESET_ECO, climate::CLIMATE_PRESET_BOOST, climate::CLIMATE_PRESET_SLEEP}) {}
    protected:
        /// Set the supported modes
        climate::ClimateTraits traits() override;
        /// Transmit via IR the state of this climate controller.
        void transmit_state() override;
        void insert_ir_byte(remote_base::RemoteTransmitData *data, const uint16_t byte);
        /// Handle received IR Buffer
        bool on_receive(remote_base::RemoteReceiveData data) override;
        /// Parse the ir received frame
        bool parse_state_frame_(const uint8_t frame[]);

        //climate::ClimateMode mode_before_{climate::CLIMATE_MODE_OFF};

        climate::ClimateMode last_mode = climate::CLIMATE_MODE_OFF;
        float last_temp = 0;
};

}  // namespace climate_ir_panasonic
}  // namespace esphome