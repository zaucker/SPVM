#ifndef SPVM_FIELD_H
#define SPVM_FIELD_H

#include "spvm_base.h"

enum {
  SPVM_FIELD_C_FLAG_PRIVATE = 1,
  SPVM_FIELD_C_FLAG_PUBLIC = 2,
};

struct SPVM_field {
  const char* name;
  const char* signature;
  SPVM_TYPE* type;
  SPVM_PACKAGE* package;
  SPVM_OP* op_field;
  SPVM_OP* op_name;
  int32_t id;
  int32_t index;
  int32_t flag;
  int32_t type_category;
  int32_t is_captured;
  int32_t offset;
  int32_t has_setter;
  int32_t has_getter;
};

SPVM_FIELD* SPVM_FIELD_new(SPVM_COMPILER* compiler);

#endif
