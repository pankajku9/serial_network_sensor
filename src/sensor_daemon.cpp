/* read config file and initialize the sensor*/
#include "sensor_daemon.h"
#include "util.h"

using namespace std;
typedef struct __sensor_cache
{
	sensor_fix **data;
	int num_sensor;
	int curr_counter_id;
	std::mutex data_mtx;
	std::thread::id read_thread_id;
	int update_client_thread_id;
}sensor_cache;

std::map<int, int> sensor_idx;
std::map<int, bool> sensor_status;
static sensor_cache fix_cache;
static std::map<int, int> id_map;


static SENSOR_CONFIG_MODE get_config_mode(unsigned int data) {
	
	SENSOR_CONFIG_MODE mode;
	switch(data)
	{	
	case 0:
		mode = CONFIG_MODE_DIRECT_SERIAL;
		break;
	case 1:
		mode = CONFIG_MODE_SERIAL_HUB;
		break;
	case 2:
		mode = CONFIG_MODE_MULTI_DROP;
		break;
	case 3:
		mode = CONFIG_MODE_CONTROLLER;
			break;
	case 4:
		mode = CONFIG_MODE_CUSTOM_BOARD;
		break;
	default:
		mode = CONFIG_MODE_DIRECT_SERIAL;
		break;
	};
	return mode;
}

void dump_cache() 
{
	for (int i = 0; i < fix_cache.curr_counter_id; i++)
	{
		for (int j = 0; j < fix_cache.num_sensor; j++)
		{
			cout << fix_cache.data[i][j].data << " ";
		}
		cout << std::endl;
	}
}
void read_fix_batch_routine()
{
	
	while (1)
	{
		int counter = fix_cache.curr_counter_id + 1;
		if (counter == COUNTER_MAX_FOR_5S)
			counter = 0;

		for (auto p : id_map) 
		{
			char data[LEN_OF_EACH_SENSOR_DATA +1];
			sensor_fix curr_fix;
			// request for a fix
			int ret = get_fix(p.second, data);
			if (ret == SENSOR_FAILURE)
			{
				DEBUG_PRINT("%s Read from sensor %d for counter %d failed\n", __FUNCTION__, p.second, counter);
				continue;
			}

			// get sensor cache index
			int idx = sensor_idx[p.first];
			curr_fix.sensor_id = p.first;
			memcpy(curr_fix.data, data, 5);
			
			// get lock, write data to sensor cache, unlock
			fix_cache.data_mtx.lock();
			memcpy(&fix_cache.data[counter][idx], &curr_fix, sizeof(sensor_fix));
			fix_cache.data_mtx.unlock();
		}
		fix_cache.curr_counter_id = counter;
		
		DEBUG_PRINT("%s Read from sensor for counter %d done\n", __FUNCTION__, counter);
		sleep(1);
	}
}

int sensor_deamon_init()
{	
	DEBUGTRACE_ENTRY();

	ifstream config_strm("sensor.config");
	int num_sensor;

	//memset(&fix_cache, 0, sizeof(sensor_cache));

	if (config_strm.is_open()) 
	{
		
		config_strm >> num_sensor;
	
		fix_cache.data = new sensor_fix*[COUNTER_MAX_FOR_5S];
		for (int i = 0; i < COUNTER_MAX_FOR_5S; i++)
			fix_cache.data[i] = new sensor_fix[num_sensor];

		if (fix_cache.data == NULL)
		{
			ERROR_PRINT("%s memory allocatio  for Cache", __FUNCTION__);
			return SENSOR_FAILURE;
		}
			
		fix_cache.num_sensor = num_sensor;
		fix_cache.curr_counter_id = -1;

		int idx = 0;
		while (idx < num_sensor) 
		{
			
			//std::cout << "idx" << idx << std::endl;
			std::string hw_identifier;
			int sensor_num;
			unsigned int config_type;
			unsigned int sensor_type;

			config_strm >> sensor_num >> hw_identifier >> config_type >> sensor_type;

			//cout << sensor_num <<" "<< hw_identifier << " "  << config_type << " "  << sensor_type <<std::endl;
			DEBUG_PRINT(" %s %d %s %d %d", __FUNCTION__, sensor_num, hw_identifier.c_str(), config_type, sensor_type);

			SENSOR_CONFIG_MODE mode = get_config_mode(config_type);
			int handle = sensor_conect(mode, hw_identifier.c_str());
			//cout <<"handle "<<handle <<std::endl;
			if (handle > 0)
			{
				id_map[sensor_num] = handle;
				sensor_idx[sensor_num] = idx;
				sensor_status[handle] = true;
				
			}
			else
			{
				ERROR_PRINT("%s file handle cannot be open", __FUNCTION__);
				return SENSOR_FAILURE;
			}
			idx++;
		}
	
		std::thread read_fix_thread(read_fix_batch_routine);
		read_fix_thread.detach();
		fix_cache.read_thread_id =  read_fix_thread.get_id();

	}
	else
	{
		ERROR_PRINT("%s file handle cannot be open", __FUNCTION__);
		return SENSOR_FAILURE;
	}
	config_strm.close();
	return SENSOR_SUCESS;
}

int sensor_deamon_exit()
{
	DEBUGTRACE_ENTRY();

	for (int i = 0; i < COUNTER_MAX_FOR_5S; i++)
		delete fix_cache.data[i];
	delete fix_cache.data;

	for (auto p : id_map) {
		sensor_disconnect(p.second);
	}
	// close thread
	return SENSOR_SUCESS;
}

int get_sensor_info(sensor_info *info)
{	
	DEBUGTRACE_ENTRY();

	int num_active = 0;
	for (auto p : id_map)
	{
		info->sensor_id[num_active] = p.first;
		num_active++;
	}
	info->active_sensor = (int)num_active;

	return SENSOR_SUCESS;
}

int set_sensor_faulty(int sensor_id)
{
	return SENSOR_SUCESS;
}
int get_sensor_status(int sensor_id)
{
	return SENSOR_SUCESS;
}
int get_all_sensor_status(int sensor_id)
{
	return SENSOR_SUCESS;
}

/* Provides last sensor fix , returns 0 on success*/
int get_sensor_fix(int sensor_id, sensor_fix *data) 
{	

	DEBUGTRACE_ENTRY();

	RETURN_IF_NULL(data, "data");

	if (id_map.find(sensor_id) == id_map.end())
		return SENSOR_FAILURE;

	if (fix_cache.curr_counter_id == -1)
		return SENSOR_FAILURE;

	int idx = sensor_idx[sensor_id];
	int counter = fix_cache.curr_counter_id;
	std::lock_guard<std::mutex> lock(fix_cache.data_mtx);
	memcpy(data, &fix_cache.data[counter][idx], sizeof(sensor_fix));

	return SENSOR_SUCESS;
}

/* Provides batch of sensor fix of all sensors */
int get_sensor_data(sensor_data * data) 
{	
	//RETURN_IF_NULL(data, "data");

	//RETURN_IF_NULL(data->data, "data");

	int batch_size = sizeof(sensor_fix) * fix_cache.num_sensor;
	int counter = fix_cache.curr_counter_id;

	std::lock_guard<std::mutex> lock(fix_cache.data_mtx);
	memcpy(data->data, &fix_cache.data[counter][0], batch_size);
	data->num_fix = (int)fix_cache.num_sensor;

	return SENSOR_SUCESS;
}

/* Provides all fix of a sensor collected in num sec */
int get_senor_data_num_sec(int sensor_id, int num_sec, sensor_data * data)
{
	return SENSOR_SUCESS;
}

/* Provides all sensor fix of all the sensor in num sec */
int get_senor_data_dump(int num_sec, sensor_data * data) {
	return SENSOR_SUCESS;
}