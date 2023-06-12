#if !defined(NXT_MAIN_H)
#define NXT_MAIN_H

#include <Arduino.h>
#include "pin_defs.h"
#include "debug.h"
#include <Preferences.h>

#include "connection_task.h"
#include "server_task.h"
#include "hardware_task.h"

#define NETWORK_CONNECTION BIT0
#define SERVER_CONNECTION BIT1
#define HARDWARE_STARTED BIT2

typedef struct {
	// basic
	char ssid[32];
	char pass[32];
	// advanced
	IPAddress localIp, gateway, subnet, dns1, dns2;
	// info
	char mac[18];
} ConnctionConfig;

extern Preferences configPreference;
extern SemaphoreHandle_t xdebug;
extern EventGroupHandle_t xevent;
extern QueueHandle_t xqueue;

extern ConnctionConfig connectionConfig;
extern int display_value;

struct Payload
{
	char data[512];
};

#endif // NXT_MAIN_H
