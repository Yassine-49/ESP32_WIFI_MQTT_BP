#include "connection_task.h"

// AsyncWebServer mserver(80);
// AsyncWebSocket msocket("/cmds");
WiFiClient wifiClient;
ConnctionConfig connectionConfig;

// void msocket_event_handler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
TaskHandle_t connection_task_handle;

uint8_t wifiConnectionRetries = WIFI_CONNECTION_RETRIES;

// function defs
void wifiCallback(WiFiEvent_t event, WiFiEventInfo_t info);
uint8_t wifiConnect();
uint8_t wifiReconnect();
void wifiDisconnect();

uint8_t nxt_connection::nxt_connection_init()
{
	return xTaskCreatePinnedToCore(nxt_connection::main, "CONNECTION", 1024 * 6, NULL, tskIDLE_PRIORITY + 1, &connection_task_handle, 1);
}

void nxt_connection::nxt_connection_deinit()
{
	if (WiFi.isConnected())
	{
		wifiDisconnect();
	}

	xEventGroupClearBits(xevent, SERVER_CONNECTION);

	if (connection_task_handle != NULL)
	{
		vTaskDelete(connection_task_handle);
	}
}

void nxt_connection::main(void *p)
{
	// setup wifi
	strcpy(connectionConfig.ssid, configPreference.getString("ssid", WIFI_SSID).c_str());
	strcpy(connectionConfig.pass, configPreference.getString("pass", WIFI_PASS).c_str());

	// STA
	WiFi.mode(WIFI_AP_STA);
	String AP_NAME = "PTL-" + WiFi.macAddress();
	AP_NAME.replace(":", "");
	WiFi.softAP(AP_NAME.c_str(), "12345678");

	// connect
	wifiConnect();

	// Events
	WiFi.onEvent(wifiCallback);

	// // DNS
	// if (MDNS.begin("ptl"))
	// 	dbgln("MDNS started");
	// else
	// 	dbgln("Failed to start MDNS");

	// Starting SPIFFS
	SPIFFS.begin();

	// // Serving files
	// mserver.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	// // Socket handler
	// msocket.onEvent(msocket_event_handler);
	// mserver.addHandler(&msocket);

	// DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
	// mserver.begin();
	// MDNS.addService("http", "tcp", 80);

	dbgln("[d] IP: " + WiFi.localIP().toString());

	while (1)
	{
		EventBits_t uxBits = xEventGroupWaitBits(xevent, NETWORK_CONNECTION, pdFALSE, pdFALSE, 1000);
		dbgln("[d] (connection) uxBits: " + String(uxBits));
		if ((uxBits & NETWORK_CONNECTION) == 0)
		{
			if (wifiConnectionRetries <= 0)
			{
				dbgln("[d] Wifi reconnection timeout, restarting");
				WiFi.disconnect(true, true);
				delay(10);
				wifiConnect();
			}
			else
			{
				wifiConnectionRetries--;
				wifiReconnect();
			}
		}
		delay(5000);
	}
	vTaskDelete(NULL);
}

void wifiCallback(WiFiEvent_t event, WiFiEventInfo_t info)
{
	dbgln("[d] Event: " + String(event));
	if (event == ARDUINO_EVENT_WIFI_STA_CONNECTED)
	{
		dbgln("[d] WiFi STA connected");

		// reset counter
		wifiConnectionRetries = WIFI_CONNECTION_RETRIES;
	}

	if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED)
	{
		dbgln("[d] WiFi STA disconnected");
		// update flag (network disconnected)
		xEventGroupClearBits(xevent, NETWORK_CONNECTION);
	}

	if (event == ARDUINO_EVENT_WIFI_STA_START)
	{
		dbgln("[d] WiFi STA start");
	}

	if (event == ARDUINO_EVENT_WIFI_STA_STOP)
	{
		dbgln("[d] WiFi STA stop");
	}

	if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP)
	{
		dbgln("[d] WiFi STA IP: " + IPAddress(info.got_ip.ip_info.ip.addr).toString());
		// update flag (network connected)
		xEventGroupSetBits(xevent, NETWORK_CONNECTION);
	}
}

uint8_t wifiConnect()
{
	if (WiFi.isConnected())
		wifiDisconnect();
	return WiFi.begin(connectionConfig.ssid, connectionConfig.pass);
}

uint8_t wifiReconnect()
{
	return WiFi.reconnect();
}

void wifiDisconnect()
{
	WiFi.disconnect(false, true);
}

// void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
// {
// 	AwsFrameInfo *info = (AwsFrameInfo *)arg;
// 	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
// 	{
// 		data[len] = 0;
// 		Serial.printf("data: %s\r\n", (char *)data);
// 		DynamicJsonDocument doc(32);
// 		deserializeJson(doc, (char *)data);
// 		JsonObject obj = doc.as<JsonObject>();
// 	}
// }

// void msocket_event_handler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {}
