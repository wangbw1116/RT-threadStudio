#include "task_led.h"
#include <rtthread.h>
#include "drivers/pin.h"

void task_led_loop(void *par)
{
	int count = 1;
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

	while (count++)
	{
		//rt_kprintf("********* led thread running\r\n");
			rt_pin_write(LED0_PIN, PIN_HIGH);
			rt_thread_mdelay(500);
			rt_pin_write(LED0_PIN, PIN_LOW);
			rt_thread_mdelay(500);
	}
}

void task_led_start()
{
	rt_thread_t tid;
	tid = rt_thread_create("led", task_led_loop, RT_NULL,
												 RT_MAIN_THREAD_STACK_SIZE, RT_MAIN_THREAD_PRIORITY+1, 20);
	rt_thread_startup(tid);
}
