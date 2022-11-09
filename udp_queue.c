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


#define POTTERY_PAGER_PREFIX udp_queue
#define POTTERY_PAGER_VALUE_TYPE udp_elem_t*
#define PPOTTERY_PAGER_PER_PAGE 256
#define POTTERY_PAGER_LIFECYCLE_MOVE_BY_VALUE 1
#include "pottery/pager/pottery_pager_static.t.h"


/**
 *
 */
udp_elem_t* udp_queue_value_set(uint32_t org_addr, uint16_t org_port, uint32_t appl_addr, uint16_t appl_port, uint8_t* md5)
{
  udp_elem_t* elem = (udp_elem_t*)malloc(sizeof(udp_elem_t));
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

  udp_queue_t client_to_server_queue;
  udp_queue_init(&client_to_server_queue);

  for (uint32_t i = 0; i < 10; i++) {
    udp_queue_insert_last(&client_to_server_queue, udp_queue_value_set(0x0a0a0a0a + i, 8080 + i, 0x0b0b0b0b + i, 7890 + i, md5));
    printf("inserted\n");
  }

  udp_queue_entry_t first = udp_queue_first(&client_to_server_queue);

#if 1
  // Print its contents
  for (uint32_t i = 0; i < udp_queue_count(&client_to_server_queue); ++i) {
    printf("%u\n", i);
    udp_elem_t* it = *udp_queue_entry_ref(&client_to_server_queue, udp_queue_at(&client_to_server_queue, i));
    printf("it=%p\n", it);
    // printf("%08x:%04x, %08x:%04x\n", it->org_addr, it->org_port, it->appl_addr, it->appl_port);
  }
#else

  // udp_queue_remove(&client_to_server_queue);
  udp_queue_entry_t it = udp_queue_begin(&client_to_server_queue);
  printf("begin\n");
  while (it != NULL) {
    printf("%08x:%04x, %08x:%04x\n", (*it)->org_addr, (*it)->org_port, (*it)->appl_addr, (*it)->appl_port);
    it = udp_queue_next(&client_to_server_queue, it);
    printf("next\n");
  }
#endif

  udp_queue_destroy(&client_to_server_queue); // frees all ints
}


/* vim: se nu ts=2 sw=2 si et : */
