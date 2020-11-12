/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-06     RealThread   first version
 */

#include <rtthread.h>
#include <board.h>
#include <drv_common.h>

RT_WEAK void rt_hw_board_init()
{
    extern void hw_board_init(char *clock_src, int32_t clock_src_freq, int32_t clock_target_freq);

    /* Heap initialization */
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *) HEAP_BEGIN, (void *) HEAP_END);
#endif

    hw_board_init(BSP_CLOCK_SOURCE, BSP_CLOCK_SOURCE_FREQ_MHZ, BSP_CLOCK_SYSTEM_FREQ_MHZ);

    /* Set the shell console output device */
#if defined(RT_USING_DEVICE) && defined(RT_USING_CONSOLE)
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Board underlying hardware initialization */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

}

void rt_hw_init_test1()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_BOARD_EXPORT(rt_hw_init_test1);

void rt_hw_init_test2()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_PREV_EXPORT(rt_hw_init_test2);

void rt_hw_init_test3()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_DEVICE_EXPORT(rt_hw_init_test3);

void rt_hw_init_test4()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_COMPONENT_EXPORT(rt_hw_init_test4);

void rt_hw_init_test5()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_ENV_EXPORT(rt_hw_init_test5);

void rt_hw_init_test6()
{
    int a = 1;
    int b = 2;
    b += a;
}
INIT_APP_EXPORT(rt_hw_init_test6);
