#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "protocol_examples_common.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_heap_caps.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_task_wdt.h"
#include "rtc_wdt.h"
#include <sys/socket.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/sys.h"

#define TAG "REALTIME"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define ESP_MAXIMUM_RETRY  5
#define ESP_WIFI_SSID "YOUR_SSID"
#define ESP_WIFI_PASS "YOUR_PASS"
#define MAX_DOCUMENT_ID_LENGTH 25
#define FIRESTORE_HTTP_PATH_SIZE 256
#define FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE 1024
#define FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE 1024
#define FIRESTORE_FIREBASE_API_KEY "API_KEY_YOUR_PROJECT"

char APP_MAIN_FIRESTORE_COLLECTION_ID[MAX_DOCUMENT_ID_LENGTH] = "test";
char APP_MAIN_FIRESTORE_DOCUMENT_ID[MAX_DOCUMENT_ID_LENGTH] = "test";
char path[MAX_DOCUMENT_ID_LENGTH];
char data_mask[50];
char data_doc[500];

EventGroupHandle_t wifi_event_group;
int retry_num = 0;

#endif
