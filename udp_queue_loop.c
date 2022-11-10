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
  // printf("inserted: elem=%p, %08x:%04x, %08x:%04x\n", elem, elem->org_addr, elem->org_port, elem->appl_addr, elem->appl_port);
  return elem;
}


/**
 *
 */
int main(int argc, char* argv[])
{
  uint8_t md5[16];
  uint32_t insert_max;
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  udp_elem_t** it;
  udp_elem_t** next;
  udp_elem_t** end;

  insert_max = strtoul(argv[1], NULL, 10);

  udp_queue_t client_to_server_queue;
  udp_queue_init(&client_to_server_queue);

  udp_queue_insert_last(&client_to_server_queue, udp_queue_new(0x0b0a0a0a, 9080, 0x0c0b0b0b, 2890, md5));

  for (uint32_t i = 0; i < insert_max; i++) {
    pthread_mutex_lock(&mtx);
    udp_queue_insert_last(&client_to_server_queue, udp_queue_new(0x0a0a0a0a + i, 8080 + (i % 100), 0x0b0b0b0b + i, 7890 + (i % 100), md5));
    pthread_mutex_unlock(&mtx);

    pthread_mutex_lock(&mtx);
    {
      it = udp_queue_begin(&client_to_server_queue);
      end = udp_queue_end(&client_to_server_queue);
      it = udp_queue_next(&client_to_server_queue, it);
      if (it != end) {
        udp_queue_remove(&client_to_server_queue, it);
      }
    }
    pthread_mutex_unlock(&mtx);
  }

#if 0
  it = udp_queue_begin(&client_to_server_queue);
  end = udp_queue_end(&client_to_server_queue);
  // printf("begin\n");
  while (it != end) {
    udp_elem_t* elem = *it;
    // printf("extract: elem=%p, %08x:%04x, %08x:%04x\n", elem, elem->org_addr, elem->org_port, elem->appl_addr, elem->appl_port);
    // printf("next\n");
    next = udp_queue_next(&client_to_server_queue, it);
    udp_queue_remove(&client_to_server_queue, it);
    it = next;
  }

#endif

  it = udp_queue_begin(&client_to_server_queue);
  end = udp_queue_end(&client_to_server_queue);
  while (it != end) {
    udp_elem_t* elem = *it;
    printf("extract: elem=%p, %08x:%04x, %08x:%04x\n", elem, elem->org_addr, elem->org_port, elem->appl_addr, elem->appl_port);
    // printf("next\n");
    it = udp_queue_next(&client_to_server_queue, it);
  }


  udp_queue_destroy(&client_to_server_queue); // frees all ints
}


/* vim: se nu ts=2 sw=2 si et : */
