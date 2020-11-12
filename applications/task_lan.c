#include "task_lan.h"
#include <rtthread.h>
#include "lwip/sockets.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers_defs.h"

char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

extern const char * iotx_ca_crt;

void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

int example_publish(void *handle)
{
    int             res = 0;
    const char     *fmt = "/%s/%s/user/get";
    char           *topic = NULL;
    int             topic_len = 0;
    char           *payload = "{\"message\":\"hello!\"}";

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

#if 0
void task_lan_loop(void *par)
{
	uint8_t buf[RECV_BUFF_LEN];
	int socket_fd, ret = 0, read_bytes = 0, conn_flag = 0;
	fd_set rfs;
	struct timeval time;
	struct sockaddr_in dest;
	
	socket_fd = lwip_socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(socket_fd < 0){
		rt_kprintf("new a socket failed\n");
		return;
	}
	
	memset(buf, 0, RECV_BUFF_LEN);
	time.tv_sec = 1;
	time.tv_usec = 0;
	
	dest.sin_family = AF_INET;
	dest.sin_port = DEST_PORT;
	dest.sin_addr.s_addr = inet_addr(DEST_IP);
	bzero(&(dest.sin_zero), 8);

	while (1)
	{
	    if(!conn_flag){
	        ret = lwip_connect(socket_fd, (struct sockaddr *)&dest, sizeof(struct sockaddr));
	        rt_kprintf("lwip_connect ret: %d\n", ret);
	        if(ret != 0){
	            rt_kprintf("connect failed\n");
	            rt_thread_mdelay(1000);
	            continue;
	        }
	        else{
	            conn_flag = 1;
	        }
	    }

		FD_ZERO(&rfs);
		FD_SET(socket_fd, &rfs);
		ret = lwip_select(socket_fd+1, &rfs, RT_NULL, RT_NULL, &time);
		if(ret){
			memset(buf, 0, RECV_BUFF_LEN);
			read_bytes = lwip_read(socket_fd, buf, RECV_BUFF_LEN);
			if(read_bytes > 0){
				for (int i = 0; i < read_bytes; i++){
					rt_kprintf("%c ", buf[i]);
				}
				rt_kprintf("\r\n");
			}
		}

		rt_thread_mdelay(100);
	}
}
#else
void task_lan_loop(void *par)
{
    uint8_t hostname[100] = {0};
    void                   *pclient = NULL;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;

    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

    EXAMPLE_TRACE("my example");

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    sprintf(hostname, "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com", DEMO_PRODUCT_KEY);
    mqtt_params.host = hostname;
    mqtt_params.port = 443;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.write_buf_size = 1024;
    mqtt_params.read_buf_size = 1024;
    mqtt_params.handle_event.h_fp = example_event_handle;
    mqtt_params.pub_key = iotx_ca_crt;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return;
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return;
    }

    while (1) {
        if (0 == loop_cnt % 20) {
            example_publish(pclient);
        }

        IOT_MQTT_Yield(pclient, 200);

        loop_cnt += 1;
    }

    return;
}
#endif

void task_lan_start()
{
	rt_thread_t tid;
	tid = rt_thread_create("lan", task_lan_loop, RT_NULL,
												 RT_MAIN_THREAD_STACK_SIZE, RT_MAIN_THREAD_PRIORITY+2, 20);
	rt_thread_startup(tid);
}
