#pragma once
#include <stdint.h>

typedef int  ret_status;
typedef enum {
	CONFIG_MODE_DIRECT_SERIAL,
	CONFIG_MODE_SERIAL_HUB,
	CONFIG_MODE_MULTI_DROP,
	CONFIG_MODE_CONTROLLER,
	CONFIG_MODE_CUSTOM_BOARD,
}SENSOR_CONFIG_MODE;

int sensor_conect(SENSOR_CONFIG_MODE config_mode, const char* hw_identifier);
int sensor_disconnect(int handle);

// non blocking, low level access
int write_command_to_read(int handle, char write_cmnd);
int read_data(int handle, char *data);

// blocking
int get_fix(int handle, char *data);


int get_sensor_state(int handle, int *status);