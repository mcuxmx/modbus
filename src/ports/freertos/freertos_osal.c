#include "mbal_sys.h"
#include "osal.h"


uint16_t os_rev16( uint16_t value )
{
    uint16_t x;
    x = ( value << 8) | (value >>8);
    return x;
}

int os_event_wait ( os_event_t * handler, uint32_t event, uint32_t *flags, uint32_t timeout )
{
    EventBits_t r_event;
    r_event = xEventGroupWaitBits(handler,   
                            event, 
                            pdFALSE,
                            pdFALSE,
                            timeout);
    *flags = (uint32_t)r_event;
    
    if( r_event & event ){
        return 0;
    }
    
    return -1;
}




os_thread_t os_thread_create( const char *name, int priority, uint16_t stack_size, void(*task)(void *), void *arg )
{
    TaskHandle_t handler;
    xTaskCreate((TaskFunction_t)task,
                (const char *)name,
                (uint16_t)stack_size,
                (void *)arg,
                (UBaseType_t)priority,
                (TaskHandle_t *)&handler);
    return handler;
}