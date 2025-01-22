#pragma once

#include "../innova.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace innova {

class InnovaSwitch : public switch_::Switch, public Component {
 public:
  InnovaSwitch(Innova *innova) : innova_(innova) {}
  void setup() override;
  void write_state(bool state) override;
  void dump_config() override;
 protected:
  Innova *innova_;
};

}  // namespace innova
}  // namespace esphome
