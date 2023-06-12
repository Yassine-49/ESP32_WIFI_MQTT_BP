#if !defined(NXT_HARDWARE_TASK_H)
#define NXT_HARDWARE_TASK_H

#include "main.h"
#include <connection_task.h>

namespace nxt_hardware
{
	uint8_t nxt_hardware_init();
	void nxt_hardware_deinit();
	void main(void *p);
}

#endif // NXT_HARDWARE_TASK_H
