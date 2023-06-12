#if !defined(NXT_CONNECTION_TASK_H)
#define NXT_CONNECTION_TASK_H

#include "main.h"
#include <WiFi.h>
#include <SPIFFS.h>
// #include <ESPmDNS.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define WIFI_CONNECTION_RETRIES 10
#define WIFI_SSID "wifi_name"
#define WIFI_PASS "wifi_pass"

// extern AsyncWebSocket msocket;
extern WiFiClient wifiClient;

namespace nxt_connection
{
	uint8_t nxt_connection_init();
	void nxt_connection_deinit();
	void main(void *p);
}

#endif // NXT_CONNECTION_TASK_H
