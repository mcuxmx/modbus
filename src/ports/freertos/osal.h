#ifndef __OSAL_H_
#define __OSAL_H_
#include "stm32f4xx_hal.h"
#include "mbal_sys.h"

typedef struct EventGroupDef_t              os_event_t;

#define os_event_create                     (os_event_t *)xEventGroupCreate
#define os_event_set(handler,event)         xEventGroupSetBits(handler,event)
#define os_event_clr(handler,event)         xEventGroupClearBits(handler,event)

typedef TaskHandle_t                        os_thread_t;
#define OS_WAIT_FOREVER                     portMAX_DELAY

#define CC_PACKED_BEGIN                     __PACKED
#define CC_PACKED_END                       
#define CC_PACKED                           __PACKED
#define CC_STATIC_ASSERT(...)
#define CC_ASSERT(...)

#define CC_TO_BE16(x)                       os_rev16((uint16_t)(x))
#define CC_FROM_BE16(x)                     os_rev16((uint16_t)(x))
#define BIT(x)          ( 1 << x )

uint16_t os_rev16( uint16_t value );
int os_event_wait ( os_event_t * handler, uint32_t event, uint32_t *flags, uint32_t timeout );
os_thread_t os_thread_create( const char *name, int priority, uint16_t stack_size, void(*task)(void *), void *arg );

#endif
