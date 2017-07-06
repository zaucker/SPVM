#ifndef SPVM_ARRAY_OBJECT_H
#define SPVM_ARRAY_OBJECT_H

#include "spvm_base.h"

// SPVM_ARRAY_OBJECT
struct SPVM_array_object {
  int8_t type;
  int8_t value_type;
  int32_t ref_count;
  int32_t length;
};

enum {
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_BYTE,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_SHORT,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_INT,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_LONG,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_FLOAT,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_DOUBLE,
  SPVM_ARRAY_OBJECT_C_VALUE_TYPE_OBJECT,
};

#endif
