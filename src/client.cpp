#include <iostream>
#include "sensor_daemon.h"
#include "util.h"
#include <thread>

int main()
{
	std::cout << "1" << std::endl;

	int ret = sensor_deamon_init();
	RETURN_ON_FAILURE(ret, "sensor init failed");

	sensor_info info;
	get_sensor_info(&info);

	std::cout << "active_sensor" << info.active_sensor << std::endl;

	std::thread emulator_thrd(start_emulator);
	emulator_thrd.detach();
	int read_counter = 0;
	while (1)
	{
		for (int i = 0; i < info.active_sensor; i++)
		{
		
			sensor_fix fix;
			get_sensor_fix(info.sensor_id[i], &fix);
			std::cout << "Counter: " << read_counter << " SensorID: " << info.sensor_id[i] <<"  Data:" << fix.data << std::endl;
		}
		read_counter++;
		int x;
		std::cout << "Enter 0 to exit else press any NUM Key to continue reading" << std::endl;
		std::cin >> x;
		if (x == 0)
			break;
	}
	
	
	//emulator_thrd.join();

	sensor_deamon_exit();
}