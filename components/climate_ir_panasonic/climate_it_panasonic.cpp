#include "climate_ir_panasonic.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_ir_panasonic {

static const char *const TAG = "climate.climate_ir_panasonic";

// Modify supported modes to quit HEAT_COOL
climate::ClimateTraits PanasonicClimate::traits(){
    auto traits = ClimateIR::traits();
    auto modes = traits.get_supported_modes();
    modes.erase(climate::CLIMATE_MODE_HEAT_COOL);
    modes.insert(climate::CLIMATE_MODE_AUTO);
    traits.set_supported_modes(modes);
    return traits;
}

// At this moment do nothing
// Just sent COOL 24º AUTO_FAN AUTO_SWING
void PanasonicClimate::transmit_state() {
    uint8_t remote_state[STATE_FRAME_SIZE] = {0};

    // Byte0
    remote_state[0] = ((uint8_t) int(this->target_temperature) - TEMP_OFFSET);
    switch(this->fan_mode.value()){
        case climate::CLIMATE_FAN_HIGH:
            remote_state[0] += 0x60;
            break;
        case climate::CLIMATE_FAN_MEDIUM:
            remote_state[0] += 0x40;
            break;
        case climate::CLIMATE_FAN_LOW:
            remote_state[0] += 0x20;
            break;
        case climate::CLIMATE_FAN_AUTO:
        default:
            remote_state[0] += 0xF0;
            break;
    }

    switch(this->mode){
        case climate::CLIMATE_MODE_OFF:
            remote_state[1] = 0x06;
            break;
        case climate::CLIMATE_MODE_HEAT:
            remote_state[1] = 0x0C;
            break;
        case climate::CLIMATE_MODE_COOL:
            remote_state[1] = 0x0A;
            break;
        case climate::CLIMATE_MODE_DRY:
            remote_state[1] = 0x0B;
            break;
        case climate::CLIMATE_MODE_AUTO:
        default:
            remote_state[1] = 0x0E;
            break;

    }
    remote_state[2] = 0xF4;
    remote_state[3] = 0x36;

    if (this->last_mode == this->mode && this->last_temp != this->target_temperature){
        remote_state[1] |= 0x10; 
    }

    this->last_mode = this->mode;
    this->last_temp = this->target_temperature;

    // //Prepare to transmit
    auto transmit = this->transmitter_->transmit();
    auto *data = transmit.get_data();
    data->set_carrier_frequency(CARRIER_FRECUENCY);
    data->reserve(142);


    // //Set data
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    insert_ir_byte(data,remote_state[0]);
    insert_ir_byte(data,remote_state[0]);
    insert_ir_byte(data,remote_state[1]);
    insert_ir_byte(data,remote_state[1]);
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    insert_ir_byte(data,remote_state[0]);
    insert_ir_byte(data,remote_state[0]);
    insert_ir_byte(data,remote_state[1]);
    insert_ir_byte(data,remote_state[1]);
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    data->item(BIT_MARK, 14025);
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    insert_ir_byte(data,remote_state[2]);
    insert_ir_byte(data,remote_state[2]);
    insert_ir_byte(data,remote_state[3]);
    insert_ir_byte(data,remote_state[3]);
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    insert_ir_byte(data,remote_state[2]);
    insert_ir_byte(data,remote_state[2]);
    insert_ir_byte(data,remote_state[3]);
    insert_ir_byte(data,remote_state[3]);
    data->item(MARK_LEAD_INOUT, MARK_LEAD_INOUT);
    data->mark(BIT_MARK);
    data->space(50000);

    transmit.perform();
}

void PanasonicClimate::insert_ir_byte(remote_base::RemoteTransmitData *data, const uint16_t byte){
    bool bit;
    for (uint8_t mask = 1; mask >0; mask <<=1){ //swap bit trough mask
        data -> mark(BIT_MARK);
        bit = byte & mask;
        data->space(bit ? ONE_SPACE : ZERO_SPACE);
    }
}

bool PanasonicClimate::on_receive(remote_base::RemoteReceiveData data){
    return true;
}

}  // namespace climate_ir_panasonic
}  // namespace esphome