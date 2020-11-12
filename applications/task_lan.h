#ifndef __TASK_LAN_H__
#define __TASK_LAN_H__
#include <board.h>

#define RECV_BUFF_LEN 1024
#define DEST_IP "192.168.1.1"
#define DEST_PORT 8888

void task_lan_start();

#endif//__TASK_LAN_H__
