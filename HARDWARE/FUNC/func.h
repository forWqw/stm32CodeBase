#ifndef __FUNC_H
#define __FUNC_H

#define DEVICE_NUM_MAX 	4
#define DEVICE_NAME 		"device%d"
#define SET_FUNCTION		"set"
#define GET_FUNCTION		"get"
enum work_status 
{
	STOP_STATUS= 0,
	WORK_STATUS
};
typedef struct
{
		char name[16];
		int num;
		int work_status;
		int old_work_status;
		int work_time;
}_deviceStatus;

#define DEVICES_BITS(x) (1 << x) 

int func_set_work_status(char *get_msg);
int deviceFunc(void);
void deviceStatusInit();
#endif