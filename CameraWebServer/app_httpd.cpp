#include "esp_http_server.h"
#include "esp_camera.h"
#include "camera_index.h"  // HTML UI pentru interfața camerei
#include "fb_gfx.h"
#include "img_converters.h"
#include "esp32-hal-log.h"

// Variabile globale pentru servere HTTP
httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

// Handler pentru streaming video
static esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;

    if (fb->format == PIXFORMAT_JPEG) {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
    } else {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    }

    esp_err_t res = httpd_resp_send(req, (const char *)_jpg_buf, _jpg_buf_len);

    if (fb) {
        esp_camera_fb_return(fb);
    } else if (_jpg_buf) {
        free(_jpg_buf);
    }
    return res;
}

// Handler pentru captură de imagine statică
static esp_err_t capture_handler(httpd_req_t *req) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

    esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return res;
}

// Funcție pentru pornirea serverului camerei
void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    // Configurare URI pentru streaming
    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };

    // Configurare URI pentru captură
    httpd_uri_t capture_uri = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = capture_handler,
        .user_ctx = NULL
    };

    // Pornire server streaming
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }

    // Pornire server captură
    config.server_port += 1;  // Diferit port pentru captură
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &capture_uri);
    }
}
