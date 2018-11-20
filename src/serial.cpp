
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "util.h"
#include <iostream>

#define error_message printf

//#define EMUL_MODE

#ifdef EMUL_MODE
static int emul_fd = -1;
#endif

static int set_interface_attribs(int fd, int speed, int parity)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0)
	{
		ERROR_PRINT("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

	tty.c_iflag &= ~IGNBRK;
	tty.c_lflag = 0;

	tty.c_oflag = 0;
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 5;

	//flow control
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);

	tty.c_cflag |= (CLOCAL | CREAD);

	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		ERROR_PRINT("error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

static void set_blocking(int fd, int should_block)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0)
	{
		ERROR_PRINT("error %d from tggetattr", errno);
		return;
	}

	tty.c_cc[VMIN] = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
		ERROR_PRINT("error %d setting term attributes", errno);
}


//int serial_connect(const char *port_name, int *file_des)
int serial_connect(const char *port_name)
{
	DEBUGTRACE_ENTRY();

	int ret_status = SENSOR_SUCESS;

	int fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		ERROR_PRINT("error %d opening %s: %s", errno, port_name, strerror(errno));
		ret_status = -SENSOR_FAILURE;
		goto FUNC_EXIT;
	}
	DEBUG_PRINT(" %s port_name: %s fd: %d %d", __FUNCTION__, port_name, fd);

FUNC_EXIT:
#ifdef EMUL_MODE
	if (emul_fd == -1 && ret_status == SENSOR_FAILURE)
		emul_fd = fd;
	else
	{
		return emul_fd;
	}
#endif
	return fd;
}

int serial_configure(int file_des)
{
	set_interface_attribs(file_des, B9600, 0);
	set_blocking(file_des, 0);
	return SENSOR_SUCESS;
}


int serial_write_byte(int file_des, char *data, int num_bytes)
{	
	DEBUGTRACE_ENTRY();

	DEBUG_PRINT("%d Write request \n", file_des);
	int ret = write(file_des, data, num_bytes);
	if (ret < 0)
	{
		ERROR_PRINT("serial_write_byte write failed \n");
		return SENSOR_FAILURE;
	}
	DEBUGTRACE_EXIT();
	return SENSOR_SUCESS;
}


int serial_read_byte(int file_des, char *data, int num_bytes) 
{
	DEBUGTRACE_ENTRY();
	int ret = read(file_des, data, num_bytes);
	if (ret < 0)
	{
		ERROR_PRINT("serial_write_byte write failed \n");
		return SENSOR_FAILURE;
	}
	DEBUG_PRINT("%d Read request done %s \n", file_des, data);
	DEBUGTRACE_EXIT();
	return SENSOR_SUCESS;
}

int serial_disconnect(int file_des) {

#ifdef EMUL_MODE
	if (emul_fd != -1)
		close(file_des);
	return SENSOR_SUCESS;
#endif

	close(file_des);
	return SENSOR_SUCESS;

}