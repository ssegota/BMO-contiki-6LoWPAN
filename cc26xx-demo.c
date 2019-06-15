#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "sys/timer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "random.h"


#include <stdio.h>
#include <stdint.h>

#include "lib/random.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"

#include "dev/leds.h"
#include "dev/watchdog.h"
#include "random.h"
#include "dev/button-sensor.h"

#include "simple-udp.h"
#include "servreg-hack.h"

#include <string.h>

int status = 0;
int counter = 0;
#define UDP_PORT 1234
#define SERVICE_ID 190

#define SEND_INTERVAL (2 * CLOCK_SECOND)
#define SEND_TIME (2 * CLOCK_SECOND)
#define LED_ON_TIME (1*CLOCK_SECOND)
// #define CC26XX_DEMO_LOOP_INTERVAL (CLOCK_SECOND * 20)
// #define CC26XX_DEMO_SENSOR_1 &button_left_sensor
static struct simple_udp_connection unicast_connection;
static struct simple_udp_connection broadcast_connection;

static struct timer rt;
static struct stimer st;
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

#define CC26XX_DEMO_SENSOR_3 &button_up_sensor


/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(cc26xx_demo_process, "cc26xx demo process");
AUTOSTART_PROCESSES(&cc26xx_demo_process);
/*---------------------------------------------------------------------------*/

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
    status = 1;
    counter = counter + 1;
    printf("Message count = %d\n", counter);
    printf("status changed to %d\n", status);
    leds_on(LEDS_RED);

    //PROCESS_YIELD();

    
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc26xx_demo_process, ev, data)
{
    //uip_ipaddr_t *ipaddr;
    uip_ipaddr_t addr;
    PROCESS_BEGIN();

    printf("CC26XX demo - modified\n");

    static struct etimer periodic_timer;
   // static struct etimer send_timer;
    //static struct etimer led_timer;
   // etimer_set(&led_timer, LED_ON_TIME);
    etimer_set(&periodic_timer, SEND_INTERVAL);
   // etimer_set(&send_timer, SEND_TIME);
    simple_udp_register(&broadcast_connection, UDP_PORT,
                        NULL, UDP_PORT,
                        receiver);
    while (1)
    {
        printf("STATUS = %d\n", status);
        //PROCESS_YIELD();
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
        
	
        //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
        //etimer_reset(&send_timer);
        if (ev == sensors_event)
        {
            if (data == &button_sensor)
            {
                printf("Button pressed (up)\n");
                status = 1;
            }
        }
        else
        {
            if(status==1){
              //  printf("ZAJEBA SE.\n");
            }
            else{
                printf("Starting message received, status is %d\n", status);
                status = 1;
            }
        }

        if (status == 0)
        {
            //PROCESS_YIELD();
            printf("STATUS = %d", status);
            //PROCESS_WAIT_EVENT();

            printf("STATUS - IDLE\n");
            /*
            if (ev == sensors_event)
            {
                if (data == CC26XX_DEMO_SENSOR_3)
                {
                    printf("Button pressed (up)\n");
                    status = 1;
                }
            }
            else
            {
                continue;
            }
            */
            //if message received
        }

        if (status == 1)
        {
            printf("STATUS - SENDER\n");
            //send message
	    leds_off(LEDS_RED);
            printf("Sending broadcast\n");

            //etimer_set(&et, CLOCK_SECOND*2);
            printf("Starting timer.\n");

            //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

            printf("Timer done.\n");
            printf("Creating local node list.\n");
            uip_create_linklocal_allnodes_mcast(&addr);
            printf("Creating local node list done.\n");
	    printf("Sending message.\n");
            simple_udp_sendto(&broadcast_connection, "Test", 4, &addr);
            status = 2;

            printf("Message sent, status set to:%d\n", status);
        }

        else if (status == 2)
        {
            printf("STATUS LISTENER\n");
            //pause
            //PROCESS_WAIT_EVENT();
            //change to sender
	    //pause
	    printf("RECEIVER DONE. STATUS=%d\n", status);
            printf("message received, status is %d\n", status);
            status = 1;
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
