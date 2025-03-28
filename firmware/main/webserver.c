#include "cJSON.h"
#include "config.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "wifi.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SSID "ESP32_AP"
#define PASSWORD "12345678"

extern cJSON *config;

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

char *eq_keys[] = {"freq", "order", "q"};
bool is_eq_key(char *find) {
  int len = sizeof(eq_keys) / sizeof(eq_keys[0]);
  int i;

  for (i = 0; i < len; ++i) {
    if (!strcmp(eq_keys[i], find)) {
      return true;
    }
  }
  return false;
}
char *setting_keys[] = {"wifi", "i2s0", "i2s1"};
bool is_setting_key(char *find) {
  int len = sizeof(setting_keys) / sizeof(setting_keys[0]);
  int i;

  for (i = 0; i < len; ++i) {
    if (!strcmp(setting_keys[i], find)) {
      return true;
    }
  }
  return false;
}
esp_err_t set_sub_keys(cJSON *config, char *query) {
  char *part = strsep(&query, "=&");

  bool skip_val = false;
  char *key = "";
  int i = 0;
  while (part != NULL) {
    ESP_LOGE("TOKEN", "part %i %s", i, part);
    if (i % 2 == 0) {
      // we have the key here
      if (strcmp("key", part) == 0 || strcmp("value", part) == 0) {
        skip_val = true;
      } else {
        key = malloc(sizeof(char) * strlen(part));
        strcpy(key, part);
      }
    } else {
      // we have the value here
      if (skip_val)
        skip_val = false;
      else {
        cJSON *item = cJSON_GetObjectItem(config, key);
        ESP_LOGE("SETTING", "%s: %s", key, part);
        free(key);
        int int_val = atoi(part);
        bool is_int = int_val != 0 || strcmp(part, "0") == 0;
        if (is_int)
          cJSON_SetIntValue(item, int_val);
        else
          cJSON_SetValuestring(item, part);
      }
    }
    i++;
    part = strsep(&query, "=&");
  };
  ESP_LOGE("TOKEN BABY", "%s", cJSON_Print(config));
  return ESP_OK;
}
static esp_err_t api_handler(httpd_req_t *req) {
  size_t query_len;
  query_len = httpd_req_get_url_query_len(req) + 1;

  char *query_str = malloc(query_len);
  httpd_req_get_url_query_str(req, query_str, query_len);

  char action[16];
  httpd_query_key_value(query_str, "action", action, 16);

  if (strcmp(action, "get_all") == 0) {
    char *resp_buf = cJSON_PrintUnformatted(config);
    free(query_str);
    return httpd_resp_send(req, resp_buf, strlen(resp_buf));
  }

  if (strcmp(action, "set") == 0) {
    char key[10];
    httpd_query_key_value(query_str, "key", key, 10);

    char value[20];
    httpd_query_key_value(query_str, "value", value, 20);

    if (is_eq_key(key)) {
      char type[10];
      httpd_query_key_value(query_str, "type", type, 10);
      char channel_str[3];
      httpd_query_key_value(query_str, "channel", channel_str, 3);
      int channel_number = atoi(channel_str);
      free(query_str);

      if (channel_number < 0 || channel_number > 1)
        return httpd_resp_send(req, "invalid channel", 15);

      cJSON *eq = cJSON_GetObjectItem(config, "eq");
      cJSON *channel = cJSON_GetArrayItem(eq, channel_number);
      cJSON *pass;

      if (strcmp(type, "lowpass") == 0)
        pass = cJSON_GetObjectItem(channel, "lowpass");
      else
        pass = cJSON_GetObjectItem(channel, "highpass");

      cJSON_SetNumberValue(cJSON_GetObjectItem(pass, key),
                           (uint16_t)atoi(value));

      return httpd_resp_send(req, "ok", 2);
    } else if (is_setting_key(key)) {
      cJSON *settings = cJSON_GetObjectItem(config, "settings");
      cJSON *setting = cJSON_GetObjectItem(settings, key);
      set_sub_keys(setting, query_str);
      httpd_resp_send(req, "ok", 2);
      save_config();
      sleep(3);
      esp_restart();
      return ESP_OK;
    }
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
