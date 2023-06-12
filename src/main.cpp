#include "main.h"

Preferences configPreference;
SemaphoreHandle_t xdebug = xSemaphoreCreateMutex();
EventGroupHandle_t xevent = xEventGroupCreate();
QueueHandle_t xqueue;

Payload payload;
uint32_t check_timer = millis();

void setup()
{
	Serial.begin(115200);
	configPreference.begin("config", false);

	xqueue = xQueueCreate(5, sizeof(Payload));

	dbgln("[d] (main) Starting Connection task");
	nxt_connection::nxt_connection_init();
	dbgln("[d] (main) Starting Hardware task");
	nxt_hardware::nxt_hardware_init();
}

void loop()
{
	// event loop
	if ((millis() - check_timer) >= 2000)
	{
		// update timer
		check_timer = millis();

		// get event flags
		EventBits_t uxBits = xEventGroupWaitBits(xevent, NETWORK_CONNECTION | SERVER_CONNECTION | HARDWARE_STARTED, pdFALSE, pdFALSE, 1000);
		dbgln("[d] (main) uxBits: " + String(uxBits));

		if ((uxBits & NETWORK_CONNECTION) == 0)
		{
			dbgln("[d] (main) Network Disonnected");
			if ((uxBits & SERVER_CONNECTION) != 0)
			{
				nxt_server::nxt_server_deinit();
			}
			nxt_connection::nxt_connection_init();
		}

		// Connected to Network
		if (((uxBits & NETWORK_CONNECTION) != 0) && ((uxBits & SERVER_CONNECTION) == 0))
		{
			dbgln("[d] (main) Network Connected");
			dbgln("[d] (main) Connecting to Server");

			nxt_server::nxt_server_init();
		}

		// Disconnected from Network
		if (((uxBits & NETWORK_CONNECTION) == 0) && ((uxBits & SERVER_CONNECTION) != 0))
		{
			dbgln("[d] (main) Network Disconnected");

			dbgln("[d] (main) Disconnecting from Server");
			nxt_server::nxt_server_deinit();

			// dbgln("[d] (main) Disabling hardware");
			// nxt_hardware::nxt_hardware_deinit();
		}

		// Connected to Network and Server
		if (((uxBits & NETWORK_CONNECTION) != 0) && ((uxBits & SERVER_CONNECTION) != 0) && ((uxBits & HARDWARE_STARTED) == 0))
		{
			dbgln("[d] (main) CONNECTION OK | SERVER OK");
		}
	}
}
