#ifndef SPVM_CONSTANT_POOL_PACKAGE_H
#define SPVM_CONSTANT_POOL_PACKAGE_H

#include "spvm_base.h"

// SPVM_CONSTANT_POOL_PACKAGE
struct SPVM_constant_pool_package {
  int32_t name_constant_pool_index;
  int32_t fields_length;
  int32_t ref_fields_length;
};

#endif
