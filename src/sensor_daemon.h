#pragma once


#pragma once
#include <stdint.h>

/*************************************************************************************************/
#define MAX_NUM_SENSOR_SUPPORTED 10
#define LEN_OF_EACH_SENSOR_DATA 5
#define MAX_DATA_TIME 5

/**************** Management and status monitoring************************************************/
/* read prop file and initlize the sensor*/
int sensor_deamon_init();
int sensor_deamon_exit();

typedef struct {
	int active_sensor;
	int sensor_id[MAX_NUM_SENSOR_SUPPORTED];
}sensor_info;

/* return the active configuration set od densor */
int get_sensor_info(sensor_info *info);

int set_sensor_faulty(int sensor_id);
int get_sensor_status(int sensor_id);
int get_all_sensor_status(int sensor_id);


/**************** state less interface***********************************************************/
typedef struct {
	int sensor_id;
	char data[LEN_OF_EACH_SENSOR_DATA +1];
}sensor_fix;

typedef struct {
	int num_fix;
	sensor_fix *data;
}sensor_data;

/* Provides last sensor fix , returns 0 on success*/
int get_sensor_fix(int sensor_id, sensor_fix *data);

/* Provides batch of sensor fix of all sensors */
int get_sensor_data(sensor_data * data);

/* Provides all fix of a sensor collected in num sec */
int get_senor_data_num_sec(int sensor_id, int num_sec, sensor_data * data);

/* Provides all sensor fix of all the sensor in num sec */
int get_senor_data_dump(int num_sec, sensor_data * data);

/**************** broad cast interface***********************************************************/
/* client need data in regular interval, make sure that they do not get repeated data*/
typedef struct {
	int num_fix;
	sensor_fix data[MAX_NUM_SENSOR_SUPPORTED];
}sensor_batch;

typedef void *sensor_callback_fn(sensor_batch *batch);
/* return a client id and call the call back function with new set of data after every sec */
int sensor_register_client(int *client_id, sensor_callback_fn func);
int sensor_unregister_client(int client_id);


/*************************************************************************************************/
// todo  time stamping of fix can be done


/*************************************************************************************************/