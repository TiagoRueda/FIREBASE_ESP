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
        ESP_LOGI(TAG, "Client HTTP_EVENT_ON_data_doc: %.*s\n", evt->data_len, (char *)evt->data);
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

void stream(void *pvParameters) {
    int bytes_read;
    char path[MAX_DOCUMENT_ID_LENGTH];
    strcpy(path, "/");
    strcat(path, APP_MAIN_FIRESTORE_DOCUMENT_ID);
    char *buffer_client = (char *)malloc(256);
    char buffer_comp[256];
    char url[FIRESTORE_HTTP_PATH_SIZE];
    snprintf(url, FIRESTORE_HTTP_PATH_SIZE, "https://%s-default-rtdb.us-central1.firebasedatabase.app%s.json",
            CONFIG_FIRESTORE_FIREBASE_PROJECT_ID, path);

    esp_http_client_config_t config = {
        .url = url,
		.auth_type = HTTP_AUTH_TYPE_NONE,
		.transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 512,
        .buffer_size_tx = 512,
		.keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Accept", "text/event-stream");

    while (1) {
        esp_err_t err;
        if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
            ESP_LOGE(TAG, "Falha ao abrir conexao HTTPs! Tentando novamente em breve...");
            esp_http_client_close(client);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Conexao estabelecida");

        esp_http_client_fetch_headers(client);
        int status_code = esp_http_client_get_status_code(client);

        if (status_code != 200) {
            ESP_LOGE(TAG, "Codigo de falha %d", status_code);
            esp_http_client_cleanup(client);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        while (1) {
        		bytes_read = esp_http_client_read(client, buffer_client, 1);

        		if (bytes_read > 0) {
        			buffer_client[bytes_read] = '\0';
        			strncat(buffer_comp, buffer_client, bytes_read);
        			if (strstr(buffer_comp, "}") != NULL || strstr(buffer_comp, "data: null") != NULL) {
        				ESP_LOGI(TAG, "buffer completo: %s", buffer_comp);
        				memset(buffer_comp, 0, sizeof(buffer_comp)); // Limpa o buffer após processar
        			}
        		}
        }

        ESP_LOGW(TAG, "Conexão fechada");
        esp_http_client_cleanup(client);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    free(buffer_client);
}

void send_realtime(const char *path, int method, const char *data_doc) {
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

void firebase_rtdb(const char *data){
    strcpy(path, "/");
    strcat(path, APP_MAIN_FIRESTORE_DOCUMENT_ID);
    char updated_path[160];
    snprintf(updated_path, sizeof(updated_path), "https://%s-default-rtdb.firebaseio.com/%s.json",
    		CONFIG_FIRESTORE_FIREBASE_PROJECT_ID, path);
    send_realtime(updated_path, HTTP_METHOD_PATCH, data);
}

void firebase_init(void) {
	snprintf(data_doc, sizeof(data_doc), "{\"test\":\"test\"}");
	firebase_rtdb(data_doc);
	xTaskCreate(stream, "streamtask", 6144/*4096*/, NULL, 18, NULL);
}
