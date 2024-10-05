#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <esp_timer.h>
#include <img_converters.h>
#include <fb_gfx.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#include <driver/rtc_io.h>

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Nastavení Wi-Fi
const char* ssid = "ssid";  // Zadejte název vaší Wi-Fi sítě
const char* password = "moje_tajne_heslo";  // Zadejte heslo vaší Wi-Fi sítě

// Piny kamery pro modul AI-THINKER ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


void setup() {
  // Iniciace sériové komunikace
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Konfigurace nastavení kamery
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Iniciace kamery
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Chyba při inicializaci kamery: 0x%x", err);
    return;
  }

  // Nastavení zrcadlení a otočení
  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 1);    // Otočení obrazu vzhůru nohama

  // Připojení k Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi připojeno");

  server.begin();
  // Start webového serveru
  server.on("/", [](){
    Serial.println("Request");
    server.send(200, "text/html", "<h1>ESP32-CAM Web Server</h1><p><img src=\"/stream\"></p>");
  });
  server.on("/stream", handle_jpg_stream);
  server.on("/photo.jpg", handle_jpg_photo);

  
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}


void handle_jpg_photo() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Chyba při pořizování snímku");
    server.send(500, "text/plain", "Chyba při pořizování snímku");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    uint8_t * jpg_buf;
    size_t jpg_buf_len;
    bool jpeg_converted = frame2jpg(fb, 80, &jpg_buf, &jpg_buf_len);
    if (!jpeg_converted) {
      Serial.println("JPEG komprese selhala");
      server.send(500, "text/plain", "JPEG komprese selhala");
      esp_camera_fb_return(fb);
      return;
    }
    server.sendHeader("Content-Type", "image/jpeg");
    server.sendHeader("Content-Length", String(jpg_buf_len));
    server.client().write((const char *)jpg_buf, jpg_buf_len);
    free(jpg_buf);
  } else {
    server.sendHeader("Content-Type", "image/jpeg");
    server.sendHeader("Content-Length", String(fb->len));
    server.client().write((const char *)fb->buf, fb->len);
  }

  esp_camera_fb_return(fb);
}


void handle_jpg_stream(void)
{
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

Serial.println("Streaming");
  // Nastavení HTTP hlaviček
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "multipart/x-mixed-replace; boundary=frame");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Chyba při pořizování snímku");
      return;
    }
    if (fb->format != PIXFORMAT_JPEG) {
      bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
      if (!jpeg_converted) {
        Serial.println("JPEG komprese selhala");
        esp_camera_fb_return(fb);
        return;
      }
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }

    // Odesílání jednotlivých snímků jako multipart/x-mixed-replace
    size_t hlen = snprintf((char *)part_buf, 64, "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", _jpg_buf_len);
    server.sendContent((const char *)part_buf, hlen);
    server.sendContent((const char *)_jpg_buf, _jpg_buf_len);
    server.sendContent("\r\n--frame\r\n");

    esp_camera_fb_return(fb);
    if (!server.client().connected()) {
      break;
    }
  }
}



