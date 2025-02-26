
#include "wifi_board.h"
#include "audio_codecs/vb6824_audio_codec.h"
#include "opus_codecs/no_opus_codec.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "iot/thing_manager.h"

#include <wifi_station.h>
#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/spi_common.h>

#define TAG "CustomBoard"

class CustomBoard : public WifiBoard {
private:
    Button boot_button_;
    VbAduioCodec audio_codec;

    void InitializeButtons() {
        boot_button_.OnDoubleClick([this]() {
            // auto& app = Application::GetInstance();
            // if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            // }
        });
        boot_button_.OnPressDown([this]() {
            Application::GetInstance().StartListening();
        });
        boot_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });
    }

    // 物联网初始化，添加对 AI 可见设备
    void InitializeIot() {
        auto& thing_manager = iot::ThingManager::GetInstance();
        thing_manager.AddThing(iot::CreateThing("Speaker"));
    }

public:
    CustomBoard() : boot_button_(BOOT_BUTTON_GPIO), audio_codec(CODEC_TX_GPIO, CODEC_RX_GPIO){          
        InitializeButtons();
        InitializeIot();
        audio_codec.OnWakeUp([this]() {
            if(Application::GetInstance().GetDeviceState() != kDeviceStateListening){
                Application::GetInstance().WakeWordInvoke("你好小智");
            }
        });
    }

    virtual AudioCodec* GetAudioCodec() override {
        return &audio_codec;
    }

#ifdef CONFIG_OPUS_CODEC_DISABLE_ESP_OPUS
    virtual OpusCodec* GetOpusCodec() override {
        static NoOpusCodec opus_codec;
        return &opus_codec;
    }
#endif
};

DECLARE_BOARD(CustomBoard);
