#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "util.h"
#include "sensor.h"
#include "serial.h"
#include <map>
#include <iostream>

static std::map<int, SENSOR_CONFIG_MODE> config_map;

int sensor_conect(SENSOR_CONFIG_MODE config_mode, const char* hw_identifier)
{	
	DEBUGTRACE_ENTRY();

	int file_desc;
	if (config_mode == CONFIG_MODE_DIRECT_SERIAL)
	{
		file_desc = serial_connect(hw_identifier);
		config_map[file_desc] = config_mode;

	}

	return file_desc;

}
int sensor_disconnect(int handle)
{	
	
	if (config_map[handle] == CONFIG_MODE_DIRECT_SERIAL)
	{
		int fd = handle;
		serial_disconnect(fd);
	}
	return SENSOR_SUCESS;
}

// non blocking, low level access
int write_command_to_read(int handle, char write_cmnd) 
{
	int ret;
	if (config_map[handle] == CONFIG_MODE_DIRECT_SERIAL)
	{
		int fd = handle;
		ret = serial_write_byte(fd, &write_cmnd, 1);
		RETURN_ON_FAILURE(ret, "write byte failed");
	}
	return SENSOR_SUCESS;
}
int read_data(int handle, char *data) {
	int ret;
	if (config_map[handle] == CONFIG_MODE_DIRECT_SERIAL)
	{
		int fd = handle;
		ret =  serial_write_byte(fd, data, 5);
		RETURN_ON_FAILURE(ret, "read byte failed");
	}
	return SENSOR_SUCESS;
}

// blocking
int get_fix(int handle, char *data)
{
	DEBUGTRACE_ENTRY();
	int ret;

	if (config_map[handle] == CONFIG_MODE_DIRECT_SERIAL)
	{	
		char write_cmnd = 'a';
		int fd = handle;
		ret = serial_write_byte(fd, &write_cmnd, 1);
		RETURN_ON_FAILURE(ret, "write byte failed");

		usleep(70000);
		ret = serial_read_byte(fd, data, 5);
		RETURN_ON_FAILURE(ret, "read byte failed");
	}
	return SENSOR_SUCESS;
}


int get_sensor_state(int handle, int *status) {
	return SENSOR_SUCESS;
}