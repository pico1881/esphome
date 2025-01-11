#include "innova.h"
#include "esphome/core/log.h"


namespace esphome {
namespace innova {

static const char *const TAG = "innova";

using namespace esphome::climate;

void Innova::dump_config() { LOG_CLIMATE("", "Innova climate", this); }

void Innova::setup() {}

void Innova::loop() {}

void Innova::control(const ClimateCall &call) {
  if (call.get_mode().has_value()) {
    ClimateMode mode = *call.get_mode();
//    InnovaPacket *pkt;
    switch (mode) {
      case climate::CLIMATE_MODE_OFF:
//        pkt = this->codec_->get_stop_request();
        break;
      case climate::CLIMATE_MODE_HEAT:
//        pkt = this->codec_->get_start_request();
        break;
      default:
        ESP_LOGW(TAG, "Unsupported mode: %d", mode);
        return;
    }
/*    auto status =
        esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_handle_,
                                 pkt->length, pkt->data, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
    if (status) {
      ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
    }*/
  }
  if (call.get_target_temperature().has_value()) {
/*    auto *pkt = this->codec_->get_set_target_temp_request(*call.get_target_temperature());
    auto status =
        esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_handle_,
                                 pkt->length, pkt->data, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
    if (status) {
      ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
    }*/
  }
}

void Innova::set_unit_of_measurement(const char *unit) { this->fahrenheit_ = !strncmp(unit, "f", 1); }

void Innova::update() {
/*  if (this->node_state != espbt::ClientState::ESTABLISHED)
    return;

  if (this->current_request_ < 2) {
    auto *pkt = this->codec_->get_read_device_status_request();
    if (this->current_request_ == 0)
      this->codec_->get_set_unit_request(this->fahrenheit_ ? 'f' : 'c');
    auto status =
        esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_handle_,
                                 pkt->length, pkt->data, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
    if (status) {
      ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
    }
    this->current_request_++; 
  }*/
}

}  // namespace innova
}  // namespace esphome
