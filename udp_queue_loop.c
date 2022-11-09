#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>


/**
 *
 */
typedef struct _udp_elem {
  uint32_t org_addr;
  uint16_t org_port;
  uint32_t appl_addr;
  uint16_t appl_port;
  uint8_t md5[16];
  time_t created_at;
} udp_elem_t;


#define POTTERY_NODE_LIST_PREFIX udp_queue
#define POTTERY_NODE_LIST_VALUE_TYPE udp_elem_t
#define POTTERY_NODE_LIST_LIFECYCLE_BY_VALUE 1
#include "pottery/node_list/pottery_node_list_static.t.h"


/**
 *
 */
void udp_queue_value_set(udp_elem_t* elem, uint32_t org_addr, uint16_t org_port, uint32_t appl_addr, uint16_t appl_port, uint8_t* md5)
{
  elem->org_addr = org_addr;
  elem->org_port = org_port;
  elem->appl_addr = appl_addr;
  elem->appl_port = appl_port;
  memcpy(elem->md5, md5, 16);
  elem->created_at = time(NULL);
}


/**
 *
 */
int main(void)
{
  uint8_t md5[16];
  uint32_t insert_max;

  insert_max = strtoul(argv[1], NULL, 10);

  udp_queue_t client_to_server_queue;
  udp_queue_init(&client_to_server_queue);

  for (uint32_t i = 0; i < insert_max; i++) {
    udp_elem_t* elem;
    udp_queue_emplace_last(&client_to_server_queue, &elem);
    udp_queue_value_set(elem, 0x0a0a0a0a + i, 8080 + (i % 100), 0x0b0b0b0b + i, 7890 + (i % 100), md5);
    //printf("inserted\n");
  }

  udp_elem_t* it = udp_queue_begin(&client_to_server_queue);
  // printf("begin\n");
  while (it != NULL) {
    // printf("%08x:%04x, %08x:%04x\n", it->org_addr, it->org_port, it->appl_addr, it->appl_port);
    it = udp_queue_next(&client_to_server_queue, it);
    break;
    // printf("next\n");
  }

  udp_queue_destroy(&client_to_server_queue); // frees all ints
}


/* vim: se nu ts=2 sw=2 si et : */
