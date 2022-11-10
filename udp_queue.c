#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <time.h>


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
#define POTTERY_NODE_LIST_VALUE_TYPE udp_elem_t*
#define POTTERY_NODE_LIST_LIFECYCLE_BY_VALUE 1
#include "pottery/node_list/pottery_node_list_static.t.h"


/**
 *
 */
udp_elem_t* udp_queue_new(uint32_t org_addr, uint16_t org_port, uint32_t appl_addr, uint16_t appl_port, uint8_t* md5)
{
  udp_elem_t* elem = (udp_elem_t*)malloc(sizeof(udp_elem_t));
  elem->org_addr = org_addr;
  elem->org_port = org_port;
  elem->appl_addr = appl_addr;
  elem->appl_port = appl_port;
  memcpy(elem->md5, md5, 16);
  elem->created_at = time(NULL);
  printf("insert:  elem=%p, %08x:%04x, %08x:%04x\n", elem, elem->org_addr, elem->org_port, elem->appl_addr, elem->appl_port);
  return elem;
}


/**
 *
 */
int main(void)
{
  uint8_t md5[16];

  udp_queue_t client_to_server_queue;
  udp_queue_init(&client_to_server_queue);

  for (uint32_t i = 0; i < 10; i++) {
    udp_queue_insert_last(&client_to_server_queue, udp_queue_new(0x0a0a0a0a + i, 8080 + i, 0x0b0b0b0b + i, 7890 + i, md5));
  }

  udp_elem_t** it = udp_queue_begin(&client_to_server_queue);
  udp_elem_t** next;
  udp_elem_t** end = udp_queue_end(&client_to_server_queue);
  printf("begin it=%p, end=%p\n", it, end);
  while (it != end) {
    udp_elem_t* elem = *it;
    printf("elem=%p\n", elem);
    printf("extract: elem=%p, %08x:%04x, %08x:%04x\n", elem, elem->org_addr, elem->org_port, elem->appl_addr, elem->appl_port);
    next = udp_queue_next(&client_to_server_queue, it);
    printf("next, it=%p\n", next);
    udp_queue_remove(&client_to_server_queue, it);
    printf("removed\n");
    it = next;
  }

  udp_queue_destroy(&client_to_server_queue); // frees all ints
}


/* vim: se nu ts=2 sw=2 si et : */
