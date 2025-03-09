#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>

#define SSID "ESP32_AP"
#define PASSWORD "12345678"

void wifi_init_softap(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = {
      .ap = {.ssid = SSID,
             .password = PASSWORD,
             .ssid_len = strlen(SSID),
             .channel = 1,
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = 4},
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI("wifi", "Access Point \"%s\" started", SSID);
}

#include "esp_http_server.h"

extern const unsigned char
    index_html_start[] asm("_binary_index_html_gz_start");
extern const unsigned char index_html_end[] asm("_binary_index_html_gz_end");

extern const unsigned char script1_js_start[] asm(
    "_binary_index_astro_astro_type_script_index_0_lang_js_gz_start");
extern const unsigned char script1_js_end[] asm(
    "_binary_index_astro_astro_type_script_index_0_lang_js_gz_end");

extern const unsigned char script2_js_start[] asm(
    "_binary_index_astro_astro_type_script_index_0_lang2_js_gz_start");
extern const unsigned char script2_js_end[] asm(
    "_binary_index_astro_astro_type_script_index_0_lang2_js_gz_end");

extern const unsigned char global_js_start[] asm("_binary_global_js_gz_start");
extern const unsigned char global_js_end[] asm("_binary_global_js_gz_end");

static esp_err_t send_file(httpd_req_t *req, const unsigned char *start,
                           const unsigned char *end, const char *content_type) {

  httpd_resp_set_type(req, content_type);
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)start, end - start);
}

static esp_err_t index_handler(httpd_req_t *req) {
  return send_file(req, index_html_start, index_html_end, "text/html");
}

static esp_err_t script1_handler(httpd_req_t *req) {
  return send_file(req, script1_js_start, script1_js_end,
                   "application/javascript");
}

static esp_err_t script2_handler(httpd_req_t *req) {
  return send_file(req, script2_js_start, script2_js_end,
                   "application/javascript");
}

static esp_err_t global_js_handler(httpd_req_t *req) {
  return send_file(req, global_js_start, global_js_end,
                   "application/javascript");
}

static esp_err_t api_handler(httpd_req_t *req) {
  char query_str[128];
  httpd_req_get_url_query_str(req, query_str, 128);
  char val[5];
  httpd_query_key_value(query_str, "foo", val, 5);
  ESP_LOGW("API CALL", "query string %s, foo %s", query_str, val);
  return httpd_resp_send(req, "ok", 2);
}

void start_webserver(void *pvParameters) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_handle_t server = NULL;

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_uri_t index_uri = {
        .uri = "/", .method = HTTP_GET, .handler = index_handler};
    httpd_uri_t script1_uri = {
        .uri = "/index.astro_astro_type_script_index_0_lang.js",
        .method = HTTP_GET,
        .handler = script1_handler};
    httpd_uri_t script2_uri = {
        .uri = "/index.astro_astro_type_script_index_0_lang2.js",
        .method = HTTP_GET,
        .handler = script2_handler};
    httpd_uri_t global_js_uri = {
        .uri = "/global.js", .method = HTTP_GET, .handler = global_js_handler};

    httpd_uri_t api_uri = {
        .uri = "/api", .method = HTTP_GET, .handler = api_handler};

    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &script1_uri);
    httpd_register_uri_handler(server, &script2_uri);
    httpd_register_uri_handler(server, &global_js_uri);
    httpd_register_uri_handler(server, &api_uri);
  }
  vTaskDelete(NULL);
}

void app_main(void) {
  nvs_flash_init();
  wifi_init_softap();
  xTaskCreatePinnedToCore(start_webserver, "webserver task", 4096, NULL, 5,
                          NULL, 0);
}
