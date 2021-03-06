#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "spvm_hash.h"
#include "spvm_hash_entry.h"
#include "spvm_hash_func.h"
#include "spvm_util_allocator.h"

SPVM_HASH* SPVM_HASH_new(int32_t table_capacity) {
  
  assert(table_capacity >= 0);

  // Create hash
  SPVM_HASH* hash = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(sizeof(SPVM_HASH));

  // Default table capacity
  if (table_capacity == 0) {
    hash->table_capacity = 1;
  }
  else {
    hash->table_capacity = table_capacity;
  }
  
  // Initialize table
  int64_t hash_table_byte_size = (int64_t)hash->table_capacity * (int64_t)sizeof(int32_t);
  hash->table = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(hash_table_byte_size);
  memset(hash->table, -1, hash->table_capacity * sizeof(int32_t));
  
  // Initialize entries
  hash->entries_capacity = 1;
  int64_t hash_entries_byte_size = (int64_t)hash->entries_capacity * (int64_t)sizeof(SPVM_HASH_ENTRY);
  hash->entries = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(hash_entries_byte_size);
  hash->entries_length = 0;
  
  // Initialize key buffer
  hash->key_buffer_capacity = 1;
  int64_t hash_key_buffer_byte_size = (int64_t)hash->key_buffer_capacity;
  hash->key_buffer = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(hash_key_buffer_byte_size);
  hash->key_buffer_length = 0;
  
  return hash;
}

void SPVM_HASH_maybe_extend_entries(SPVM_HASH* hash) {
  
  assert(hash);
  
  int32_t entries_length = hash->entries_length;
  
  assert(entries_length >= 0);
  
  int32_t entries_capacity = hash->entries_capacity;
  
  if (entries_length >= entries_capacity) {
    int32_t new_entries_capacity = entries_capacity * 2;
    
    int64_t new_entries_byte_size = (int64_t)new_entries_capacity * (int64_t)sizeof(SPVM_HASH_ENTRY);
    SPVM_HASH_ENTRY* new_entries = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(new_entries_byte_size);
    memcpy(new_entries, hash->entries, entries_capacity * sizeof(SPVM_HASH_ENTRY));
    free(hash->entries);
    hash->entries = new_entries;
    
    hash->entries_capacity = new_entries_capacity;
  }
}

void SPVM_HASH_maybe_extend_key_buffer(SPVM_HASH* hash, int32_t length) {
  
  assert(hash);
  
  int32_t key_buffer_length = hash->key_buffer_length;
  
  assert(key_buffer_length >= 0);
  
  int32_t key_buffer_capacity = hash->key_buffer_capacity;
  
  if (key_buffer_length + length + (int32_t)sizeof(int32_t) >= key_buffer_capacity) {
    int32_t new_key_buffer_capacity = (key_buffer_length + length + sizeof(int32_t)) * 2;
    
    int64_t new_key_buffer_byte_size = (int64_t)new_key_buffer_capacity;
    char* new_key_buffer = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(new_key_buffer_byte_size);
    memcpy(new_key_buffer, hash->key_buffer, key_buffer_capacity);
    free(hash->key_buffer);
    hash->key_buffer = new_key_buffer;

    hash->key_buffer_capacity = new_key_buffer_capacity;
  }
}

void SPVM_HASH_free(SPVM_HASH* hash) {
  
  assert(hash);
  
  free(hash->table);
  free(hash->entries);
  free(hash->key_buffer);
  free(hash);
}

int32_t SPVM_HASH_new_hash_entry(SPVM_HASH* hash, const char* key, int32_t key_length, void* value) {
  
  assert(hash);
  assert(key);
  
  int32_t index = hash->entries_length;
  
  SPVM_HASH_maybe_extend_entries(hash);
  
  SPVM_HASH_maybe_extend_key_buffer(hash, key_length);
  
  hash->entries[index].key_index = hash->key_buffer_length;
  
  // Copy key length
  memcpy(&hash->key_buffer[hash->key_buffer_length], &key_length, sizeof(int32_t));
  
  // Copy key
  memcpy(&hash->key_buffer[hash->key_buffer_length + sizeof(int32_t)], key, key_length);
  
  hash->key_buffer_length += sizeof(int32_t) + key_length;
  
  hash->entries[index].value = value;
  hash->entries[index].next_index = -1;
  
  hash->entries_length++;
  
  return index;
}

void SPVM_HASH_rehash(SPVM_HASH* hash, int32_t new_table_capacity) {
  
  assert(hash);
  assert(new_table_capacity > 0);
  
  // Create new hash
  SPVM_HASH* new_hash = SPVM_HASH_new(new_table_capacity);
  
  // Rehash
  {
    int32_t i;
    for (i = 0; i < hash->entries_length; i++) {
      int32_t key_length;
      memcpy(&key_length, &hash->key_buffer[hash->entries[i].key_index], sizeof(int32_t));
      const char* key = &hash->key_buffer[hash->entries[i].key_index + sizeof(int32_t)];
      
      void* value = hash->entries[i].value;
      
      SPVM_HASH_insert_norehash(new_hash, key, key_length, value);
    }
  }
  
  // Replace hash fields
  free(hash->table);
  free(hash->entries);
  free(hash->key_buffer);
  hash->entries_length = new_hash->entries_length;
  hash->table_capacity = new_hash->table_capacity;
  hash->entries_capacity = new_hash->entries_capacity;
  hash->table = new_hash->table;
  hash->entries = new_hash->entries;
  
  hash->key_buffer_capacity = new_hash->key_buffer_capacity;
  hash->key_buffer_length = new_hash->key_buffer_length;
  hash->key_buffer = new_hash->key_buffer;
  
  free(new_hash);
}

void SPVM_HASH_insert_norehash(SPVM_HASH* hash, const char* key, int32_t length, void* value) {
  
  assert(hash);
  assert(key);
  assert(length >= 0);
  
  int32_t hash_value = SPVM_HASH_FUNC_calc_hash_for_index(key, length);
  int32_t table_index = hash_value % hash->table_capacity;
  
  assert(hash->table[table_index] >= -1);
  
  if (hash->table[table_index] != -1) {
    
    int32_t entry_index = hash->table[table_index];
    while (1) {
      int32_t match = 0;
      int32_t key_length;
      memcpy(&key_length, &hash->key_buffer[hash->entries[entry_index].key_index], sizeof(int32_t));
      if (key_length == 0 && length == 0) {
        match = 1;
      }
      else if (key_length == length && memcmp(key, &hash->key_buffer[hash->entries[entry_index].key_index + sizeof(int32_t)], length) == 0) {
        match = 1;
      }
      
      if (match) {
        hash->entries[entry_index].value = value;
        break;
      }
      else {
        if (hash->entries[entry_index].next_index != -1) {
          entry_index = hash->entries[entry_index].next_index;
        }
        else {
          int32_t new_entry_index = SPVM_HASH_new_hash_entry(hash, key, length, value);
          hash->entries[entry_index].next_index = new_entry_index;
          break;
        }
      }
    }
  }
  else {
    int32_t new_entry_index = SPVM_HASH_new_hash_entry(hash, key, length, value);
    hash->table[table_index] = new_entry_index;
  }
}

void SPVM_HASH_insert(SPVM_HASH* hash, const char* key, int32_t length, void* value) {
  
  assert(hash);
  assert(key);
  assert(length >= 0);
  
  // Rehash
  if (hash->entries_length > hash->table_capacity * 0.75) {
    int32_t new_table_capacity = (hash->table_capacity * 2) + 1;
    
    SPVM_HASH_rehash(hash, new_table_capacity);
  }
  
  SPVM_HASH_insert_norehash(hash, key, length, value);
}

void* SPVM_HASH_fetch(SPVM_HASH* hash, const char* key, int32_t length) {
  
  assert(hash);
  assert(length >= 0);
  
  int32_t hash_value = SPVM_HASH_FUNC_calc_hash_for_index(key, length);
  int32_t table_index = hash_value % hash->table_capacity;
  
  int32_t entry_index = -1;
  if (hash->table[table_index] != -1) {
    entry_index = hash->table[table_index];
  }
  while (1) {
    assert(entry_index >= -1);
    if (entry_index != -1) {
      int32_t match = 0;
      int32_t key_length;
      memcpy(&key_length, &hash->key_buffer[hash->entries[entry_index].key_index], sizeof(int32_t));
      if (length == 0 && key_length == 0) {
        match = 1;
      }
      else if (key_length == length && memcmp(key, &hash->key_buffer[hash->entries[entry_index].key_index + sizeof(int32_t)], length) == 0) {
        match = 1;
      }
      
      if (match) {
        return hash->entries[entry_index].value;
      }
      else {
        if (hash->entries[entry_index].next_index == -1) {
          entry_index = -1;
        }
        else {
          entry_index = hash->entries[entry_index].next_index;
        }
      }
    }
    else {
      return NULL;
    }
  }
}
