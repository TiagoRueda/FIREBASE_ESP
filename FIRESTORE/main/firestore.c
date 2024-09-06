#include "definitions.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "Client HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

void send_firestore(const char *path, int method, const char *data_doc) {
	esp_http_client_config_t config = {
			.url = path,
            .host = CONFIG_FIRESTORE_HOSTNAME,
			.port = 443,
            .method = method,
			.timeout_ms = 2000,
            .auth_type = HTTP_AUTH_TYPE_NONE,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .buffer_size = FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE,
            .buffer_size_tx = FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE,
            .event_handler = _http_event_handler,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (!client) {
		ESP_LOGE(TAG, "Failed to initialize HTTP client");
		return;
	}

	if (data_doc != NULL) {
		esp_http_client_set_post_field(client, data_doc, strlen(data_doc));
		esp_http_client_set_header(client, "Content-Type", "application/json");
	}

	esp_err_t err = esp_http_client_perform(client);
	if (err != ESP_OK) {
	    ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
	} else {
	    int status_code = esp_http_client_get_status_code(client);
	    if (status_code != 200) {
	        ESP_LOGE(TAG, "Código de falha %d", status_code);
	    }
	}

	esp_http_client_close(client);
	esp_http_client_cleanup(client);
}

void post_doc(const char *data){
	snprintf(path, sizeof(path),
				 "/v1/projects/%s/databases/(default)/documents/%s?key=%s",
                 CONFIG_FIRESTORE_FIREBASE_PROJECT_ID,
				 APP_MAIN_FIRESTORE_COLLECTION_ID,
                 FIRESTORE_FIREBASE_API_KEY);
	snprintf(data_doc, sizeof(data_doc), "{\"fields\":{\"test\":{\"stringValue\":\"test\"}}}");
	send_firestore(path, HTTP_METHOD_POST, data_doc);
}

void patch_data(const char *data, const char *mask){
	snprintf(path, sizeof(path),
                 "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
                 CONFIG_FIRESTORE_FIREBASE_PROJECT_ID,
				 APP_MAIN_FIRESTORE_COLLECTION_ID,
				 APP_MAIN_FIRESTORE_DOCUMENT_ID,
                 FIRESTORE_FIREBASE_API_KEY);
	snprintf(data_mask, sizeof(data_mask),
                 "&updateMask.fieldPaths=%s", mask);
	strncat(path, data_mask, sizeof(path) - strlen(path) - 1);
	snprintf(data_doc, sizeof(data_doc), "{\"fields\":{\"test\":{\"stringValue\":\"test123\"}}}");
	send_firestore(path, HTTP_METHOD_PATCH, data_doc);
}

void get_doc(){
	snprintf(path, sizeof(path),
                 "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
                 CONFIG_FIRESTORE_FIREBASE_PROJECT_ID,
				 APP_MAIN_FIRESTORE_COLLECTION_ID,
				 APP_MAIN_FIRESTORE_DOCUMENT_ID,
                 FIRESTORE_FIREBASE_API_KEY);
	send_firestore(path, HTTP_METHOD_GET, NULL);
}

void delete_doc(){
	snprintf(path, sizeof(path),
                 "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
                 CONFIG_FIRESTORE_FIREBASE_PROJECT_ID,
				 APP_MAIN_FIRESTORE_COLLECTION_ID,
				 APP_MAIN_FIRESTORE_DOCUMENT_ID,
                 FIRESTORE_FIREBASE_API_KEY);
	send_firestore(path, HTTP_METHOD_DELETE, NULL);
}

void firebase_init(void) {
	snprintf(data_doc, sizeof(data_doc), "{\"fields\":{\"test\":{\"stringValue\":\"test\"}}}");
	post_doc(data_doc);
	vTaskDelay(1000);
	snprintf(data_doc, sizeof(data_doc), "{\"fields\":{\"test\":{\"stringValue\":\"test123\"}}}");
	patch_data(data_doc, "test");
    vTaskDelay(1000);
	get_doc();
    vTaskDelay(1000);
	delete_doc();
}
