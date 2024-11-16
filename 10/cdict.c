/*
 * cdict.c
 * 
 * Dictionary based on a hash table utilizing open addressing to
 * resolve collisions.
 *
 * Author: John Bosco Thuo
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "cdict.h"


#define DEBUG

#define DEFAULT_DICT_CAPACITY 8
#define REHASH_THRESHOLD  0.6
#define UINT_MAX ((unsigned int)-1)

typedef enum {
  SLOT_UNUSED = 0,
  SLOT_IN_USE,
  SLOT_DELETED
} CDictSlotStatus;
  
struct _hash_slot {
  CDictSlotStatus status;
  CDictKeyType    key;
  CDictValueType  value;
};

struct _dictionary {
  unsigned int num_stored;
  unsigned int num_deleted;
  unsigned int capacity;
  struct _hash_slot *slot;
};


/*
 * Return a pseudorandom hash of a key with reasonable distribution
 * properties. Based on Python's implementation before Python 3.4
 *
 * Parameters:
 *   str   The string to be hashed
 *   capacity  The capacity of the dictionary
 * 
 * Returns: The hash, in the range 0-(capacity-1) inclusive
 */
static unsigned int _CD_hash(CDictKeyType str, unsigned int capacity)
{
  unsigned int x;
  unsigned int len = 0;

  for (const char *p = str; *p; p++) 
    len++;

  if (len == 0)
    return 0;

  const char *p = str;
  x = (unsigned int)*p << 7;

  for (int i=0; i < len; i++)
    x = (1000003 * x) ^ (unsigned int) *p++;

  x ^= (unsigned int) len;

  return x % capacity;
}



/*
 * Rehash the dictionary, doubling its capacity
 *
 * Parameters:
 *   dict     The dictionary to rehash
 * 
 * Returns: None
 */
static void _CD_rehash(CDict dict)
{
  unsigned int old_capacity = dict->capacity;
  struct _hash_slot *old_slots = dict->slot;
  
  // Double the capacity and allocate new slots
  dict->capacity *= 2;
  dict->slot = malloc(dict->capacity * sizeof(struct _hash_slot));
  assert(dict->slot != NULL);
  
  // Initialize new slots
  for (unsigned int i = 0; i < dict->capacity; i++) {
    dict->slot[i].status = SLOT_UNUSED;
    dict->slot[i].key = NULL;
    dict->slot[i].value = NULL;
  }
  
  // Reset counts since we'll re-add everything
  unsigned int old_stored = dict->num_stored;
  dict->num_stored = 0;
  dict->num_deleted = 0;
  
  // Reinsert all existing elements
  for (unsigned int i = 0; i < old_capacity; i++) {
    if (old_slots[i].status == SLOT_IN_USE) {
      CD_store(dict, old_slots[i].key, old_slots[i].value);
    }
  }
  
  assert(dict->num_stored == old_stored);
  free(old_slots);
}


// Documented in .h file
CDict CD_new()
{
  CDict dict = malloc(sizeof(struct _dictionary));
  assert(dict != NULL);
  
  dict->capacity = DEFAULT_DICT_CAPACITY;
  dict->num_stored = 0;
  dict->num_deleted = 0;
  
  dict->slot = malloc(dict->capacity * sizeof(struct _hash_slot));
  assert(dict->slot != NULL);
  
  for (unsigned int i = 0; i < dict->capacity; i++) {
    dict->slot[i].status = SLOT_UNUSED;
    dict->slot[i].key = NULL;
    dict->slot[i].value = NULL;
  }
  
  return dict;
}


// Documented in .h file
void CD_free(CDict dict)
{
  assert(dict != NULL);
  free(dict->slot);
  free(dict);
}


// documented in .h file
unsigned int CD_size(CDict dict)
{
#ifdef DEBUG
  // iterate across slots, counting number of keys found
  int used = 0;
  int deleted = 0;
  for (int i=0; i < dict->capacity; i++)
    if (dict->slot[i].status == SLOT_IN_USE)
      used++;
    else if (dict->slot[i].status == SLOT_DELETED)
      deleted++;

  assert(used == dict->num_stored);
  assert(deleted == dict->num_deleted);
#endif

  return dict->num_stored;
}


// documented in .h file
unsigned int CD_capacity(CDict dict)
{
  assert(dict != NULL);
  return dict->capacity;
}


// Documented in .h file
bool CD_contains(CDict dict, CDictKeyType key)
{
  assert(dict != NULL);
  assert(key != NULL);
  
  unsigned int index = _CD_hash(key, dict->capacity);
  unsigned int start_index = index;
  
  do {
    if (dict->slot[index].status == SLOT_UNUSED) {
      return false;
    }
    if (dict->slot[index].status == SLOT_IN_USE && 
        strcmp(dict->slot[index].key, key) == 0) {
      return true;
    }
    index = (index + 1) % dict->capacity;
  } while (index != start_index);
  
  return false;
}


// Documented in .h file
void CD_store(CDict dict, CDictKeyType key, CDictValueType value)
{
  assert(dict);
  assert(key);
  assert(value);

  // Check if rehash is needed
  if ((double)(dict->num_stored + dict->num_deleted + 1) / dict->capacity > REHASH_THRESHOLD) {
    _CD_rehash(dict);
  }
  
  unsigned int index = _CD_hash(key, dict->capacity);
  unsigned int first_deleted = UINT_MAX;
  unsigned int start_index = index;
  
  // Find existing key or first available slot
  do {
    if (dict->slot[index].status == SLOT_UNUSED) {
      // Found unused slot - use first deleted if we found one, otherwise use this
      if (first_deleted != UINT_MAX) {
        index = first_deleted;
      }
      dict->slot[index].key = key;
      dict->slot[index].value = value;
      dict->slot[index].status = SLOT_IN_USE;
      dict->num_stored++;
      if (first_deleted != UINT_MAX) {
        dict->num_deleted--;
      }
      return;
    }
    
    if (dict->slot[index].status == SLOT_DELETED && first_deleted == UINT_MAX) {
      first_deleted = index;
    }
    
    if (dict->slot[index].status == SLOT_IN_USE && 
        strcmp(dict->slot[index].key, key) == 0) {
      // Found existing key - update value
      dict->slot[index].value = value;
      return;
    }
    
    index = (index + 1) % dict->capacity;
  } while (index != start_index);
  
  // We never get here since we rehash when load factor gets too high
  assert(false);
}


// Documented in .h file
CDictValueType CD_retrieve(CDict dict, CDictKeyType key)
{
  assert(dict);
  assert(key);

  unsigned int index = _CD_hash(key, dict->capacity);
  unsigned int start_index = index;
  
  do {
    if (dict->slot[index].status == SLOT_UNUSED) {
      return INVALID_VALUE;
    }
    if (dict->slot[index].status == SLOT_IN_USE && 
        strcmp(dict->slot[index].key, key) == 0) {
      return dict->slot[index].value;
    }
    index = (index + 1) % dict->capacity;
  } while (index != start_index);
  
  return INVALID_VALUE;
}


// Documented in .h file
void CD_delete(CDict dict, CDictKeyType key)
{
  assert(dict);
  assert(key);

  unsigned int index = _CD_hash(key, dict->capacity);
  unsigned int start_index = index;
  
  do {
    if (dict->slot[index].status == SLOT_UNUSED) {
      return;
    }
    if (dict->slot[index].status == SLOT_IN_USE && 
        strcmp(dict->slot[index].key, key) == 0) {
      dict->slot[index].status = SLOT_DELETED;
      dict->slot[index].key = NULL;
      dict->slot[index].value = NULL;
      dict->num_stored--;
      dict->num_deleted++;
      return;
    }
    index = (index + 1) % dict->capacity;
  } while (index != start_index);
}


// Documented in .h file
double CD_load_factor(CDict dict)
{
  assert(dict);
  
  return (double)(dict->num_stored + dict->num_deleted) / dict->capacity;
}


// Documented in .h file
void CD_print(CDict dict)
{
  assert(dict);
  printf("*** capacity: %u stored: %u deleted: %u load_factor: %.2f\n",
         dict->capacity, dict->num_stored, dict->num_deleted, CD_load_factor(dict));
         
  for (unsigned int i = 0; i < dict->capacity; i++) {
    printf("%02u: ", i);
    switch (dict->slot[i].status) {
      case SLOT_UNUSED:
        printf("unused\n");
        break;
      case SLOT_DELETED:
        printf("DELETED\n");
        break;
      case SLOT_IN_USE:
        printf("IN_USE key=%s hash=%u value=%s\n",
               dict->slot[i].key, 
               _CD_hash(dict->slot[i].key, dict->capacity),
               dict->slot[i].value);
        break;
    }
  }
}


void CD_foreach(CDict dict, CD_foreach_callback callback, void *cb_data)
{
  assert(dict != NULL);
  assert(callback != NULL);
  
  for (unsigned int i = 0; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_IN_USE) {
      callback(dict->slot[i].key, dict->slot[i].value, cb_data);
    }
  }
}
