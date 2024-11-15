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

  // Double the capacity
  dict->capacity *= 2;
  dict->slot = calloc(dict->capacity, sizeof(struct _hash_slot));
  assert(dict->slot);

  // Rehash all the elements
  dict->num_stored = 0;
  dict->num_deleted = 0;
  for (unsigned int i = 0; i < old_capacity; i++) {
    if (old_slots[i].status == SLOT_IN_USE) {
      CD_store(dict, old_slots[i].key, old_slots[i].value);
    }
  }

  free(old_slots);
}


// Documented in .h file
CDict CD_new()
{
  CDict dict = calloc(1, sizeof(struct _dictionary));
  assert(dict);

  dict->capacity = DEFAULT_DICT_CAPACITY;
  dict->slot = calloc(dict->capacity, sizeof(struct _hash_slot));
  assert(dict->slot);

  return dict;
}


// Documented in .h file
void CD_free(CDict dict)
{
  for (unsigned int i = 0; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_IN_USE) {
      free((void*)dict->slot[i].key);
      free((void*)dict->slot[i].value);
    }
  }
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
  return dict->capacity;
}


// Documented in .h file
bool CD_contains(CDict dict, CDictKeyType key)
{
  unsigned int hash = _CD_hash(key, dict->capacity);
  for (unsigned int i = hash; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return false;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0)
      return true;
  }
  for (unsigned int i = 0; i < hash; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return false;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0)
      return true;
  }
  return false;
}


// Documented in .h file
void CD_store(CDict dict, CDictKeyType key, CDictValueType value)
{
  // assert(dict);
  // assert(key);
  // assert(value);
  unsigned int hash = _CD_hash(key, dict->capacity);
  for (unsigned int i = hash; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_UNUSED || dict->slot[i].status == SLOT_DELETED) {
      dict->slot[i].status = SLOT_IN_USE;
      dict->slot[i].key = strdup(key);
      dict->slot[i].value = strdup(value);
      dict->num_stored++;
      if (dict->slot[i].status == SLOT_DELETED)
        dict->num_deleted--;
      if (((float)dict->num_stored + (float)dict->num_deleted) / (float)dict->capacity >= REHASH_THRESHOLD)
        _CD_rehash(dict);
      return;
    } else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0) {
      free((void*)dict->slot[i].value);
      dict->slot[i].value = strdup(value);
      return;
    }
  }
  for (unsigned int i = 0; i < hash; i++) {
    if (dict->slot[i].status == SLOT_UNUSED || dict->slot[i].status == SLOT_DELETED) {
      dict->slot[i].status = SLOT_IN_USE;
      dict->slot[i].key = strdup(key);
      dict->slot[i].value = strdup(value);
      dict->num_stored++;
      if (dict->slot[i].status == SLOT_DELETED)
        dict->num_deleted--;
      if (((float)dict->num_stored + (float)dict->num_deleted) / (float)dict->capacity >= REHASH_THRESHOLD)
        _CD_rehash(dict);
      return;
    } else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0) {
      free((void*)dict->slot[i].value);
      dict->slot[i].value = strdup(value);
      return;
    }
  }
}


// Documented in .h file
CDictValueType CD_retrieve(CDict dict, CDictKeyType key)
{
  // assert(dict);
  // assert(key);

  unsigned int hash = _CD_hash(key, dict->capacity);
  for (unsigned int i = hash; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return INVALID_VALUE;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0)
      return dict->slot[i].value;
  }
  for (unsigned int i = 0; i < hash; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return INVALID_VALUE;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0)
      return dict->slot[i].value;
  }
  return INVALID_VALUE;
}


// Documented in .h file
void CD_delete(CDict dict, CDictKeyType key)
{
  // assert(dict);
  // assert(key);

  unsigned int hash = _CD_hash(key, dict->capacity);
  for (unsigned int i = hash; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0) {
      dict->slot[i].status = SLOT_DELETED;
      free((void*)dict->slot[i].key);
      free((void*)dict->slot[i].value);
      dict->num_stored--;
      dict->num_deleted++;
      return;
    }
  }
  for (unsigned int i = 0; i < hash; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      return;
    else if (dict->slot[i].status == SLOT_IN_USE && strcmp(dict->slot[i].key, key) == 0) {
      dict->slot[i].status = SLOT_DELETED;
      free((void*)dict->slot[i].key);
      free((void*)dict->slot[i].value);
      dict->num_stored--;
      dict->num_deleted++;
      return;
    }
  }
}


// Documented in .h file
double CD_load_factor(CDict dict)
{
  // assert(dict);
  return (double)(dict->num_stored + dict->num_deleted) / (double)dict->capacity;
}


// Documented in .h file
void CD_print(CDict dict)
{
  assert(dict);

  printf("*** capacity: %d stored: %d deleted: %d load_factor: %.2f\n",
         dict->capacity, dict->num_stored, dict->num_deleted,
         CD_load_factor(dict));
  for (unsigned int i = 0; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_UNUSED)
      printf("%02d: unused\n", i);
    else if (dict->slot[i].status == SLOT_DELETED)
      printf("%02d: DELETED\n", i);
    else
      printf("%02d: IN_USE key=%s hash=%d value=%s\n",
             i, dict->slot[i].key, _CD_hash(dict->slot[i].key, dict->capacity), dict->slot[i].value);
  }
}


void CD_foreach(CDict dict, CD_foreach_callback callback, void *cb_data)
{
  assert(dict);
  for (unsigned int i = 0; i < dict->capacity; i++) {
    if (dict->slot[i].status == SLOT_IN_USE) {
      callback(dict->slot[i].key, dict->slot[i].value, cb_data);
    }
  }
}
