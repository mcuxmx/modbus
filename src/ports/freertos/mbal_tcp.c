/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2015 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/


#include <stdio.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"

#include "api.h"
#include "ip4_addr.h"
#include "netif.h"

#define PAYLOAD_BUFF_SIZE       ( 1024 )

static struct netconn *tcp_conn = NULL;
static struct netconn *remote_conn = NULL;
static uint8_t payload_buf[PAYLOAD_BUFF_SIZE];
static uint16_t payload_len = 0;
static uint16_t read_ptr = 0;

static void close_conn( struct netconn *tcp_conn )
{
    if(tcp_conn != NULL)
	{
		netconn_close(tcp_conn);
		netconn_delete(tcp_conn);
        osDelay(1000);
		tcp_conn = NULL;
	}
}

int os_tcp_connect (const char * name, uint16_t port)
{
    err_t err;
    ip_addr_t ip_addr;
    
    if( 0 == ipaddr_aton(name, &ip_addr) )
    {
        return -1;
    }
    
    if( tcp_conn == NULL ){
        tcp_conn = netconn_new(NETCONN_TCP);
    }
    if( tcp_conn == NULL ){
        return -1;
    }
    
    
    
    err = netconn_connect( tcp_conn, (ip_addr_t *)&ip_addr, port);
    if (err  != ERR_OK ){
        printf("os_tcp_connect fail:%d\r\n", err);
        //close_conn( tcp_conn );
        return -1;
    }
    
    return 1;
}

int os_tcp_accept_connection (uint16_t port)
{
    err_t err;
    if( tcp_conn == NULL ){
        tcp_conn = netconn_new(NETCONN_TCP);
    }
    if( tcp_conn == NULL ){
        return -1;
    }
    
    err = netconn_bind(tcp_conn, NULL, port);
    if (err == ERR_OK)
    {
        /* Put the connection into LISTEN state */
        netconn_listen(tcp_conn);
        //netconn_set_recvtimeout(modH->conn, 1); // this is necessary to make it non blocking
    }
    else{
        close_conn( tcp_conn );
        return -1;
    }
    
    err = netconn_accept( tcp_conn, &remote_conn);
    if(err != ERR_OK)
    {
        close_conn( tcp_conn );
        return -1;
    }
    else
    {
        return 0;
    }
 
}

void os_tcp_close (int peer)
{
   close_conn(tcp_conn);
}

int os_tcp_send (int peer, const void * buffer, size_t size)
{
    err_enum_t err;
    struct netvector  xNetVectors[1];
    size_t uBytesWritten;
    const uint8_t *p;
    
    p = (const uint8_t *)buffer;
    
    xNetVectors[0].len = size;
    xNetVectors[0].ptr = (void *)buffer;
    
    printf("os_tcp_send: %d bytes:\r\n", size);
    for( int i = 0; i < size; i++ )
    {
        printf("%02x ", p[i] );
    }
    printf("\r\n");
          
    netconn_set_sendtimeout( tcp_conn, 1000 );
    err = netconn_write_vectors_partly( tcp_conn, xNetVectors, 1, NETCONN_COPY, &uBytesWritten);
    if (err != ERR_OK )
    {
        close_conn(tcp_conn);
        return -1;
    }
    
    /* Connection closed or error sending */
    return uBytesWritten;
}

int os_tcp_recv (int peer, void * buffer, size_t size)
{
    int n = 0;
    uint8_t *p = buffer;
    
    printf("os_tcp_recv: ");
    while( read_ptr < payload_len )
    {
        p[n] = payload_buf[read_ptr];
        printf("%02x ", p[n]);
        read_ptr ++;
        n++;
        if( n == size ){
            break;
        }
    }
    
    printf("\r\n");

    /* Connection closed or error receiving */
    return n;
}

int os_tcp_recv_wait (int peer, uint32_t tmo)
{
    struct netbuf *inbuf;
    err_t recv_err;
    char *buf;
    
    
    read_ptr = 0;
    payload_len = 0;
    netconn_set_recvtimeout(tcp_conn, tmo);
    recv_err = netconn_recv(tcp_conn, &inbuf);
    if (recv_err == ERR_CLSD) //the connection was closed
    {
        close_conn(tcp_conn);
        return -1;
    }
    if (recv_err == ERR_TIMEOUT) //No new data
    {
        //close_conn(tcp_conn);
        return -1;
    }
    
    if (recv_err == ERR_OK)
    {
        if (netconn_err(tcp_conn) == ERR_OK)
        {
            /* Read the data from the port, blocking if nothing yet there.
            We assume the request (the part we care about) is in one netbuf */
            netbuf_data(inbuf, (void**)&buf, &payload_len);
            
            if( payload_len > 0 ){
                memcpy( payload_buf, buf, payload_len );
            }
            netbuf_delete(inbuf); // delete the buffer always
        }
    }
    uint8_t *p = payload_buf;
    printf("os_tcp_recv_wait: %d bytes:\r\n", payload_len);
    for( int i = 0; i < payload_len; i++ )
    {
        printf("%02x ", p[i] );
    }
    printf("\r\n");
    
    return payload_len;
}
