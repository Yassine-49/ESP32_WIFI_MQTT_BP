#if !defined(NXT_SERVER_TASK_H)
#define NXT_SERVER_TASK_H

#include "main.h"

#define SERVER_BUFFER_SIZE (1024*2)
#define SERVER_KEEP_ALIVE (60*2)

#define MQTT_SERVER         "test.mosquitto.org"
#define MQTT_PORT			1883
#define SERVER_EVENT_TOPIC  "topic/*/events"
#define SERVER_INFO_TOPIC   "topic/*/info"
#define SERVER_RPC_TOPIC    "topic/*/rpc"

#define SERVER_RECONNECTION_TIME 1000

typedef struct
{
	// server
	char domain[32];
	int port;
	// auth
	char clientid[32];
	char username[32];
	char password[32];
	// topics
	char events_topic[64];
	char rpc_topic[64];
	char info_topic[64];
} ServerConfig;

extern ServerConfig serverConfig;

namespace nxt_server
{
	uint8_t nxt_server_init();
	void nxt_server_deinit();
	void main(void *p);
}

#endif // NXT_SERVER_TASK_H

