#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"
#include "simple-udp.h"
#include "servreg-hack.h"

//port used for sending/receiving messages
#define UDP_PORT 1234
//length of time between messages
#define SEND_INTERVAL (2 * CLOCK_SECOND)
//Statuses
#define STATUS_IDLE 0
#define STATUS_SENDER 1
#define STATUS_RECEIVER 2

//variable for storing current mote status
int status = STATUS_IDLE;
//variable for storing number of message received
int message_counter = 0;
//connection data
static struct simple_udp_connection broadcast_connection;

/*---------------------------------------------------------------------------*/
PROCESS(bmo_ping_pong, "bmo ping pong");
AUTOSTART_PROCESSES(&bmo_ping_pong);
/*---------------------------------------------------------------------------*/
/*
* Function that's called after "receive message" event
* Handles the message received
*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

    printf("Data received on port %d from port %d with length %d\n",
           receiver_port, sender_port, datalen);
    //change mote status
    status = STATUS_SENDER;
    //increase received message counter
    message_counter = message_counter + 1;
    printf("Message count = %d\n", message_counter);
    printf("status changed to %d\n", status);
    //turn the LED on
    leds_on(LEDS_RED);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bmo_ping_pong, ev, data)
{
    uip_ipaddr_t addr;
    PROCESS_BEGIN();

    printf("PING PONG protocol demonstration\n");
    //define timer
    static struct etimer periodic_timer;
    //set timer
    etimer_set(&periodic_timer, SEND_INTERVAL);
    //register UDP protocol service
    simple_udp_register(&broadcast_connection, UDP_PORT,
                        NULL, UDP_PORT,
                        receiver);
    while (1)
    {
        printf("STATUS = %d\n", status);
	//wait until timer expires    
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        //reset the timer after it expires
        etimer_reset(&periodic_timer);
        
        if (status == STATUS_IDLE)
        {
            printf("STATUS = %d", status);
            printf("STATUS - IDLE\n");
        }

        if (status == STATUS_SENDER)
        {
            printf("STATUS - SENDER\n");
            leds_off(LEDS_RED);
            printf("Sending broadcast\n");
            printf("Creating local node list.\n");
	    //find neighbours
            uip_create_linklocal_allnodes_mcast(&addr);
            printf("Creating local node list done.\n");
	    printf("Sending message.\n");
            //send message using defined connection
	    /*
	     * connection to send data on
	     * message data
	     * message data size
	     * address to send on
	    */
	    simple_udp_sendto(&broadcast_connection, "Ping", 4, &addr);
	    //change status to receiver
            status = STATUS_RECEIVER;
            printf("Message sent, status set to:%d\n", status);
        }
        else if (status == STATUS_RECEIVER)
        {
            printf("STATUS LISTENER\n");
	    printf("RECEIVER DONE. STATUS=%d\n", status);
            printf("message received, status is %d\n", status);
	    //change status to sender
            status = STATUS_SENDER;
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
