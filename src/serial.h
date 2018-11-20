#pragma once

#include "util.h"

int serial_connect(const char *port_name);
int serial_disconnect(int file_des);
int serial_configure(int file_des);

int serial_write_byte(int file_des, char *data, int num_bytes);

int serial_read_byte(int file_des, char *data, int num_bytes);