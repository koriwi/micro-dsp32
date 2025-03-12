#include "cJSON.h"
#include "config.h"
#include "esp_heap_caps.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#define SSID "ESP32_AP"
#define PASSWORD "12345678"

extern config_t config;

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
  size_t query_len;
  query_len = httpd_req_get_url_query_len(req) + 1;

  char *query_str = malloc(query_len);
  httpd_req_get_url_query_str(req, query_str, query_len);

  char action[10];
  httpd_query_key_value(query_str, "action", action, 10);

  if (strcmp(action, "get_all") == 0) {
    cJSON *json = cJSON_CreateObject();
    cJSON *lp = cJSON_AddObjectToObject(json, "lowpass");
    cJSON *hp = cJSON_AddObjectToObject(json, "highpass");

    cJSON_AddNumberToObject(lp, "freq", config.lp_freq);
    cJSON_AddNumberToObject(lp, "order", config.lp_order);
    cJSON_AddNumberToObject(lp, "q", config.lp_q / 32768.0f);
    cJSON_AddNumberToObject(hp, "freq", config.hp_freq);
    cJSON_AddNumberToObject(hp, "order", config.hp_order);
    cJSON_AddNumberToObject(hp, "q", config.hp_q / 32768.0f);

    char *resp_buf = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    free(query_str);
    return httpd_resp_send(req, resp_buf, strlen(resp_buf));
  }

  if (strcmp(action, "set") == 0) {
    char key[10];
    httpd_query_key_value(query_str, "key", key, 10);

    char value[10];
    httpd_query_key_value(query_str, "value", value, 10);

    char type[10];
    httpd_query_key_value(query_str, "type", type, 10);
    free(query_str);

    if (strcmp(key, "freq") == 0 && strcmp(type, "lowpass") == 0) {
      config.lp_freq = (uint16_t)atoi(value);
    } else if (strcmp(key, "freq") == 0 && strcmp(type, "highpass") == 0) {
      config.hp_freq = (uint16_t)atoi(value);
    } else if (strcmp(key, "q") == 0 && strcmp(type, "lowpass") == 0) {
      config.lp_q = (uint16_t)(atof(value) * 32768);
    } else if (strcmp(key, "q") == 0 && strcmp(type, "highpass") == 0) {
      config.hp_q = (uint16_t)atof(value) * 32768;
    } else if (strcmp(key, "order") == 0 && strcmp(type, "lowpass") == 0) {
      config.lp_order = (uint16_t)atoi(value);
    } else if (strcmp(key, "order") == 0 && strcmp(type, "highpass") == 0) {
      config.hp_order = (uint16_t)atoi(value);
    } else
      return httpd_resp_send(req, "invalid", 7);

    return httpd_resp_send(req, "ok", 2);
  }

  free(query_str);

  if (strcmp(action, "save") == 0) {
    save_config();
    return httpd_resp_send(req, "ok", 2);
  }

  return httpd_resp_send(req, "unknown", 7);
}

void start_webserver() {
  httpd_config_t httpd_config = HTTPD_DEFAULT_CONFIG();
  httpd_handle_t server = NULL;

  if (httpd_start(&server, &httpd_config) == ESP_OK) {
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
}
