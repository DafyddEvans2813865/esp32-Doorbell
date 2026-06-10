#include "camera.h"

esp_err_t cam_init_err = ESP_OK;
const char *active_pinmap = "ESP32S3_EYE";
uint16_t sensor_pid = 0;

bool cam_ok = false;
bool psram_ok = false;
const char *serverUrl = "http://192.168.0.23:5080/upload";

bool initCamera()
{
    Serial.println("Boot OK - starting camera init");
    psram_ok = psramFound();
    Serial.printf("PSRAM found: %s\n", psram_ok ? "yes" : "no");

    camera_config_t config = {};
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
    config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_XGA; - will move to once sd card
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.sccb_i2c_port = 0;

    cam_init_err = esp_camera_init(&config);
    cam_ok = (cam_init_err == ESP_OK);

    if (cam_ok)
    {
        sensor_t *s = esp_camera_sensor_get();
        if (s)
        {
            sensor_pid = s->id.PID;
        }
        Serial.printf("Camera init OK using pinmap: %s\n", active_pinmap);
        Serial.printf("Detected sensor PID: 0x%04x\n", sensor_pid);
    }
    else
    {
        Serial.printf("CAM INIT FAIL: 0x%x (%s)\n", cam_init_err, esp_err_to_name(cam_init_err));
        return false;
    }
    return true;
}

bool doesCameraWork()
{
    if (!cam_ok)
    {
        Serial.printf("camera not working: init=0x%x (%s), psram=%s\n", cam_init_err, esp_err_to_name(cam_init_err), psram_ok ? "yes" : "no");
        delay(2000);
        return false;
    }
    return true;
}

void sendCameraFrame()
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb || fb->len == 0 || fb->len > 250000)
    {
        if (fb)
            esp_camera_fb_return(fb); // free frame if too large
        return;
    }

    // release buffer
    esp_camera_fb_return(fb);

    uint8_t *buf = fb->buf;
    size_t len = fb->len;

    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");
    http.setTimeout(10000); // might remove

    int responseCode = http.POST(buf, len);

    if (responseCode > 0)
    {
        Serial.printf("Upload success: %d\n", responseCode);
    }
    else
    {
        Serial.printf("Upload failed: %s\n", http.errorToString(responseCode).c_str());
    }

    http.end();
    return;
}