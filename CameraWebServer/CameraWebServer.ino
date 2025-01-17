#include "esp_camera.h"
#include <WiFi.h>

// Declarația funcției definite în app_httpd.cpp
void startCameraServer();

#define CAMERA_MODEL_AI_THINKER  // Selectează modelul camerei
#include "camera_pins.h"

const char* ssid = "Galaxy S22+ 86F2";  // Numele rețelei WiFi
const char* password = "qble3434";      // Parola WiFi

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // Configurare cameră
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;  // Ajustează rezoluția pentru performanță
    config.jpeg_quality = 12;
    config.fb_count = 1;

    if (psramFound()) {
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected!");

    startCameraServer();  // Pornire server pentru streaming și captură

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
}

void loop() {
    delay(10000);  // Menține serverul activ
}
