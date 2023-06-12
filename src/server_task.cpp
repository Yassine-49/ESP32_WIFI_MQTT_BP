#include "server_task.h"
#include "PubSubClient.h"
#include <ArduinoJson.h>

TaskHandle_t server_task_handle;

// default
ServerConfig serverConfig;
PubSubClient mqttClient(wifiClient);

uint32_t serverConnectTimer = millis();
bool mqttConnected = false;

// function defs
uint8_t serverConnect();
void serverDisconnect();
void callback(char *topic, byte *payload, unsigned int length);
String resolveTopic(String topic, String id);

uint8_t nxt_server::nxt_server_init()
{
	dbgln("[d] STARTING SERVER TASK");
	return xTaskCreatePinnedToCore(nxt_server::main, "SERVER", 1024 * 6, NULL, tskIDLE_PRIORITY + 1, &server_task_handle, 1);
}

void nxt_server::nxt_server_deinit()
{
	if (server_task_handle != NULL)
	{
		dbgln("[D] DELETING SERVER TASK");
		vTaskDelete(server_task_handle);
	}
}

void nxt_server::main(void *p)
{
	// setup
	String mac = WiFi.macAddress();
	mac.replace(":", "");
	strcpy(serverConfig.domain, configPreference.getString("host", MQTT_SERVER).c_str());
	serverConfig.port = configPreference.getInt("port", MQTT_PORT);
	strcpy(serverConfig.clientid, configPreference.getString("clientid", "").c_str());
	strcpy(serverConfig.username, configPreference.getString("username", "").c_str());
	strcpy(serverConfig.password, configPreference.getString("password", "").c_str());
	strcpy(serverConfig.events_topic, configPreference.getString("events", "").c_str());
	strcpy(serverConfig.rpc_topic, configPreference.getString("rpc", "").c_str());
	strcpy(serverConfig.info_topic, configPreference.getString("info", "").c_str());
	dbgln("[d] ClientID: " + String(serverConfig.clientid));
	dbgln("[d] Username: " + String(serverConfig.username));
	dbgln("[d] Password: " + String(serverConfig.password));
	dbgln("[d] Event topic: " + String(serverConfig.events_topic));
	dbgln("[d] RPC topic: " + String(serverConfig.rpc_topic));
	dbgln("[d] Info topic: " + String(serverConfig.info_topic));

	// server config
	mqttClient.setServer(serverConfig.domain, serverConfig.port);
	mqttClient.setBufferSize(SERVER_BUFFER_SIZE);
	mqttClient.setKeepAlive(SERVER_KEEP_ALIVE);
	mqttClient.setCallback(callback);

	while (1)
	{
		if ((millis() - serverConnectTimer) >= SERVER_RECONNECTION_TIME)
		{
			serverConnectTimer = millis();
			if (mqttClient.state() != MQTT_CONNECTED)
			{
				xEventGroupClearBits(xevent, SERVER_CONNECTION);
				mqttConnected = false;
				if (serverConnect())
				{
					// subscribe
					dbgln("Subscribing");
					mqttClient.subscribe(serverConfig.rpc_topic);
					xEventGroupSetBits(xevent, SERVER_CONNECTION);
					mqttConnected = 1;
				}
			}
		}

		if (mqttConnected)
		{
			uint8_t messages_in_queue = uxQueueMessagesWaiting(xqueue);

			if (messages_in_queue > 0)
			{
				dbgln("[d] Messages in queue: " + String(messages_in_queue));
				Payload payload;
				if (xQueueReceive(xqueue, &payload, 10) == pdPASS)
				{
					dbgln("[d] Publishing: " + String(payload.data));
					if (mqttClient.publish(serverConfig.events_topic, payload.data))
					{
						dbgln("[d] Published");
					}
					else
					{
						dbgln("[d] Not published");
						xQueueSend(xqueue, &payload, 10);
						delay(1000);
					}
				}
			}
			mqttClient.loop();
		}
		delay(10);
	}
	vTaskDelete(NULL);
}

uint8_t serverConnect()
{
	return mqttClient.connect(serverConfig.clientid, serverConfig.username, serverConfig.password);
}

void serverDisconnect()
{
	mqttClient.disconnect();
}

void callback(char *topic, byte *payload, unsigned int length)
{
	char p[length + 1];
	memcpy(p, payload, length);
	String raw_payload = "";
	for (unsigned int i = 0; i < length; i++)
	{
		raw_payload += p[i];
	}

	DynamicJsonDocument json_packet(1024);
	deserializeJson(json_packet, raw_payload);

	// HANDLE CALLBACK DATA
}

// utils

String resolveTopic(String topic, String id)
{
	return topic.substring(0, topic.indexOf('*')) + id + topic.substring(topic.indexOf('*') + 1, topic.length());
}
