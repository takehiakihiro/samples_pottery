#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>


/*
 * First let's define a person struct.
 */
typedef struct _tcp_napt {
  uint64_t key;
  uint32_t org_addr;
  uint16_t org_port;
  uint32_t appl_addr;
  uint16_t appl_port;
} tcp_napt_t;


/**
 *
 */
static uint64_t generate_key(uint32_t addr, uint16_t port)
{
  uint64_t key = ntohs(port);
  key <<= 32;
  key |= ntohl(addr);
  return key;
}


/**
 *
 */
static tcp_napt_t* tcp_napt_new(int32_t org_addr, uint16_t org_port, uint32_t appl_addr, uint16_t appl_port)
{
  tcp_napt_t* record = (tcp_napt_t*)malloc(sizeof(tcp_napt_t));
  uint64_t key = generate_key(appl_addr, appl_port);
  record->key = key;
  record->org_addr = org_addr;
  record->org_port = org_port;
  record->appl_addr = appl_addr;
  record->appl_port = appl_port;
  return record;
}


/**
 *
 */
static void tcp_napt_delete(tcp_napt_t* record) {
  free(record);
}


/**
 *
 */
int tcp_napt_compare(uint64_t left, uint64_t right)
{
  if (left == right) {
    return 0;
  }
  else if (left > right) {
    return -1;
  }
  return 1;
}


/*
 * Now we instantiate a map of strings to persons.
 *
 * This example uses tree_map. You could also use an array_map (with or
 * without an ordering comparison), an open_hash_map (with a hash function),
 * etc.
 */

// We'll call our map "tcp_napt_map".
#define POTTERY_TREE_MAP_PREFIX tcp_napt_map

// We'll use `const char*` as the key, not `char*`, that way we can search it
// with const strings.
#define POTTERY_TREE_MAP_KEY_TYPE uint64_t
#define POTTERY_TREE_MAP_COMPARE_LESS(x,y) ((x) < (y))
// #define POTTERY_TREE_MAP_COMPARE_BY_VALUE 1

// We are storing `person_t*` as the value type so the ref type (and entry type
// for most maps) is a double-pointer `person_t**`.
#define POTTERY_TREE_MAP_VALUE_TYPE tcp_napt_t*
#define POTTERY_TREE_MAP_REF_KEY(record) (*record)->key

// We want to be able to insert() and extract() our person pointers. This
// requires that they are passable as arguments and return values; in other
// words they must be movable by value. Pointers are bitwise movable.
#define POTTERY_TREE_MAP_LIFECYCLE_MOVE_BY_VALUE 1

// We'd like our map to own the persons it contains so it needs to be able to
// destroy them when removing them. We'll be able to call remove() or destroy
// the whole map and it will delete values for us.
#define POTTERY_TREE_MAP_LIFECYCLE_DESTROY(record) tcp_napt_delete(*record)

#include "pottery/tree_map/pottery_tree_map_static.t.h"



/*
 * Here's an example of how you could use the above map.
 */

int main(int argc, char* argv[]) {
  uint32_t insert_max;
  uint32_t select_max;
  uint32_t target_addr;
  uint16_t target_port;
  tcp_napt_t** ref;
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

  insert_max = strtoul(argv[1], NULL, 10);
  select_max = strtoul(argv[2], NULL, 10);
  inet_pton(AF_INET, argv[3], &target_addr);
  target_port = strtoul(argv[4], NULL, 10);

  tcp_napt_map_t tcp_napt_table;
  tcp_napt_map_init(&tcp_napt_table);

  // Add some tcp_napt to the map
  for (uint32_t i = 0; i < insert_max; i++) {
    pthread_mutex_lock(&mtx);
    tcp_napt_map_insert(&tcp_napt_table, tcp_napt_new(htonl(0x0a000001 + i), htons(2020 + (i % 10)), htonl(0x01010101 + i), htons(8080 + (i % 10))));
    pthread_mutex_unlock(&mtx);
  }

#if 0
  POTTERY_FOR_EACH(ref, tcp_napt_map, &tcp_napt_table) {
    printf(" %016lx, %08x, %04x, %08x, %04x\n", (*ref)->key, ntohl((*ref)->org_addr), ntohs((*ref)->org_port), ntohl((*ref)->appl_addr), ntohs((*ref)->appl_port));
  }
#endif

  for (uint32_t i = 0; i < select_max; i++) {
    pthread_mutex_lock(&mtx);
    uint64_t k = generate_key(htonl(target_addr), htons(target_port));
    // printf("find key=%016lx\n", k);
    ref = tcp_napt_map_find(&tcp_napt_table, generate_key(htonl(target_addr), htons(target_port)));
#if 0
    if (tcp_napt_map_entry_exists(&tcp_napt_table, ref)) {
      printf("%016lx is %08x, %04x, %08x, %04x\n", (*ref)->key, (*ref)->org_addr, (*ref)->org_port, (*ref)->appl_addr, (*ref)->appl_port);
    }
#endif
    pthread_mutex_unlock(&mtx);
  }

  // Destroy the map. This deletes the remaining persons (dave).
  // printf("destroying map containing dave.\n");
  // tcp_napt_map_destroy(&tcp_napt_table);
  return EXIT_SUCCESS;
}


/* vim: se nu ts=2 sw=2 si et : */
