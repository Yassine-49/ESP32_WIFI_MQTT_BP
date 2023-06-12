#include "hardware_task.h"

TaskHandle_t hardware_task_handle;


uint8_t nxt_hardware::nxt_hardware_init() {
	xEventGroupSetBits(xevent, HARDWARE_STARTED);
	return xTaskCreatePinnedToCore(nxt_hardware::main, "HARDWARE", 1024 * 6, NULL, tskIDLE_PRIORITY + 1, &hardware_task_handle, 1);
}

void nxt_hardware::nxt_hardware_deinit() {
	xEventGroupClearBits(xevent, HARDWARE_STARTED);
	if (hardware_task_handle != NULL) {
		vTaskDelete(hardware_task_handle);
	}
}

void nxt_hardware::main(void *p) {
	// setup
	
	while (1) {
		// loop
	}
}

