#ifndef SPVM_API_H
#define SPVM_API_H

#include <stdint.h>

struct SPVM_api {
  int32_t (*get_array_length)(void*, void*);
};

#endif
