#include "esphome/core/log.h"
#include "innova_switch.h"

namespace esphome {
namespace innova {
static const char *TAG = "innova.switch";


void InnovaSwitch::setup() {
    innova_->add_key_lock_callback([this](bool state) {
        publish_state(state);
    });
}

void InnovaSwitch::write_state(bool state) {
    innova_->set_key_lock(state); 
}

void InnovaSwitch::dump_config(){
    LOG_SWITCH("", "Innova Switch", this);
}

}  // namespace innova
}  // namespace esphome
