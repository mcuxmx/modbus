#include "mbus.h"
#include "mb_tcp.h"
#include "mb_rtu.h"
#include "mb_bsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "osal.h"



void tcp_rtu_master_start( void *arg )
{
    const mb_tcp_cfg_t  tcp_cfg = {
        .port = 502
    };
    static mbus_cfg_t mb_master_cfg = {
      .timeout = 1000,
   };
    mbus_t * mbus;
    int slave = -1;
    mb_transport_t * tcp;
    printf("modbus tcp rtu master tart...\r\n");
    tcp = mb_tcp_init (&tcp_cfg);
    mbus = mbus_create (&mb_master_cfg, tcp);

    //slave = mbus_connect (mbus, "192.168.140.23");

    mb_address_t address = MB_ADDRESS (3, 2);
    int error;
    uint16_t value[16];
    
    osDelay(2000);
    while( 1 )
    {
        if( slave != -1 )
        {
            printf("read %x\r\n", address);
            error = mbus_read (mbus, 1, address, 1, &value);
        
            if (error)
            {
                printf ("Modbus function failed (%s).\n", mb_error_literal (error));
                mbus_disconnect(mbus,slave);
                slave = -1;
            }
            else 
            {
                printf ("0x%04x\n", value[0]);
            }
        }else{
            
            printf( "\r\nconnect to server\r\n" );
            slave = mbus_connect (mbus, "192.168.140.23");
        }
        
        osDelay(500);
    }
}

void tcp_rtu_master( void )
{
    /* Start slave task */
   os_thread_create (
      "tMbMaster",
      osPriorityLow,
      512,
      tcp_rtu_master_start,
      NULL);
}