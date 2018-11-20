
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>
#include <iostream>

#include "util.h"
//// socat -d -d pty,raw,echo=0 pty,raw,echo=0


int start_emulator()
{	
		static int counter = 0;
		char read_cmd, write_cmd = 'a', response[6] = { "XXXXX" };
		const char *portname = "/dev/pts/3";
		int fd;
		struct timeval tv;
		int retval = 0;
		fd_set rfds;

		

		//fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
		fd = open(portname, O_RDWR);
		if (fd < 0)
		{
			DEBUG_PRINT("error %d opening %s: %s", errno, portname, strerror(errno));
			//return 0;
		}
		DEBUG_PRINT(" ENULATOR connected on port  %s  listening to  %d \n", portname, fd);
		while (1) 
		{
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 70000;
			retval = select(1, &rfds, NULL, NULL, &tv);
			DEBUG_PRINT(" EMULATOR waking up fd: %d ret:%d  time:%ld \n", fd, retval, tv.tv_sec);
			DEBUG_PRINT("EMULATOR checking for read request\n");
			read(fd, &read_cmd, 1);
			if (read_cmd == write_cmd)
			{	
				sprintf(response, "SX%d", counter);
				write(fd, &response, 5);
				DEBUG_PRINT(" EMULATOR writing response for read request : %s\n", response);
			}
			counter = counter + 1;
			if (counter > 256)
				counter = 0;

			if(retval == 0 )
				DEBUG_PRINT(" Select retuned 0 \n");
			sleep(1);
		}
}