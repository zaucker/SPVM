#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <complex.h>

#include "spvm_compiler.h"
#include "spvm_op.h"

#include "spvm_runtime.h"
#include "spvm_runtime_api.h"
#include "spvm_object.h"
#include "spvm_native.h"

#include "spvm_list.h"
#include "spvm_hash.h"

#include "spvm_opcode.h"

#include "spvm_runtime_basic_type.h"
#include "spvm_runtime_package.h"
#include "spvm_runtime_sub.h"
#include "spvm_runtime_field.h"
#include "spvm_runtime_package_var.h"
#include "spvm_runtime_arg.h"
#include "spvm_runtime_field.h"
#include "spvm_runtime_weaken_backref.h"
#include "spvm_runtime_info.h"

// Only use constant
#include "spvm_package.h"
#include "spvm_basic_type.h"
#include "spvm_type.h"
#include "spvm_sub.h"
#include "spvm_limit.h"

SPVM_ENV* SPVM_BOOTSTRAP_new_env() {

  void* env_init[]  = {
    (void*)(intptr_t)offsetof(SPVM_RUNTIME, package_vars_heap), // runtime_package_vars_heap_offset
    (void*)(intptr_t)sizeof(SPVM_OBJECT), // object_header_byte_size
    (void*)(intptr_t)offsetof(SPVM_OBJECT, weaken_backref_head), // weaken_backref_head
    (void*)(intptr_t)offsetof(SPVM_OBJECT, ref_count), // object_ref_count_offset
    (void*)(intptr_t)offsetof(SPVM_OBJECT, basic_type_id), // object_basic_type_id_offset
    (void*)(intptr_t)offsetof(SPVM_OBJECT, type_dimension), // object_type_dimension_offset
    (void*)(intptr_t)offsetof(SPVM_OBJECT, runtime_type_category), // object_runtime_type_category_offset
    (void*)(intptr_t)offsetof(SPVM_OBJECT, flag), // object_flag_offset
    (void*)(intptr_t)offsetof(SPVM_OBJECT, length), // object_length_offset
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_BYTE_OBJECT, // byte_object_basic_type_id
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_SHORT_OBJECT, // short_object_basic_type_id
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_INT_OBJECT, // int_object_basic_type_id
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_LONG_OBJECT,  // long_object_basic_type_id
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_FLOAT_OBJECT, // float_object_basic_type_id
    (void*)(intptr_t)SPVM_BASIC_TYPE_C_ID_DOUBLE_OBJECT, // double_object_basic_type_id
    NULL, // runtime
    NULL, // exception_object
    NULL, // native_mortal_stack
    NULL, // native_mortal_stack_top
    NULL, // native_mortal_stack_capacity
    SPVM_RUNTIME_API_basic_type_id,
    SPVM_RUNTIME_API_field_id,
    SPVM_RUNTIME_API_field_offset,
    SPVM_RUNTIME_API_pkgvar_id,
    SPVM_RUNTIME_API_sub_id,
    SPVM_RUNTIME_API_method_sub_id,
    SPVM_RUNTIME_API_new_object_raw,
    SPVM_RUNTIME_API_new_object,
    SPVM_RUNTIME_API_new_byte_array_raw,
    SPVM_RUNTIME_API_new_byte_array,
    SPVM_RUNTIME_API_new_short_array_raw,
    SPVM_RUNTIME_API_new_short_array,
    SPVM_RUNTIME_API_new_int_array_raw,
    SPVM_RUNTIME_API_new_int_array,
    SPVM_RUNTIME_API_new_long_array_raw,
    SPVM_RUNTIME_API_new_long_array,
    SPVM_RUNTIME_API_new_float_array_raw,
    SPVM_RUNTIME_API_new_float_array,
    SPVM_RUNTIME_API_new_double_array_raw,
    SPVM_RUNTIME_API_new_double_array,
    SPVM_RUNTIME_API_new_object_array_raw,
    SPVM_RUNTIME_API_new_object_array,
    SPVM_RUNTIME_API_new_muldim_array_raw,
    SPVM_RUNTIME_API_new_muldim_array,
    SPVM_RUNTIME_API_new_mulnum_array_raw,
    SPVM_RUNTIME_API_new_mulnum_array,
    SPVM_RUNTIME_API_new_string_raw,
    SPVM_RUNTIME_API_new_string,
    SPVM_RUNTIME_API_new_string_len_raw,
    SPVM_RUNTIME_API_new_string_len,
    SPVM_RUNTIME_API_new_pointer_raw,
    SPVM_RUNTIME_API_new_pointer,
    SPVM_RUNTIME_API_concat_raw,
    SPVM_RUNTIME_API_concat,
    SPVM_RUNTIME_API_new_stack_trace_raw,
    SPVM_RUNTIME_API_new_stack_trace,
    SPVM_RUNTIME_API_length,
    SPVM_RUNTIME_API_get_elems_byte,
    SPVM_RUNTIME_API_get_elems_short,
    SPVM_RUNTIME_API_get_elems_int,
    SPVM_RUNTIME_API_get_elems_long,
    SPVM_RUNTIME_API_get_elems_float,
    SPVM_RUNTIME_API_get_elems_double,
    SPVM_RUNTIME_API_get_elem_object,
    SPVM_RUNTIME_API_set_elem_object,
    SPVM_RUNTIME_API_get_field_byte,
    SPVM_RUNTIME_API_get_field_short,
    SPVM_RUNTIME_API_get_field_int,
    SPVM_RUNTIME_API_get_field_long,
    SPVM_RUNTIME_API_get_field_float,
    SPVM_RUNTIME_API_get_field_double,
    SPVM_RUNTIME_API_get_field_object,
    SPVM_RUNTIME_API_set_field_byte,
    SPVM_RUNTIME_API_set_field_short,
    SPVM_RUNTIME_API_set_field_int,
    SPVM_RUNTIME_API_set_field_long,
    SPVM_RUNTIME_API_set_field_float,
    SPVM_RUNTIME_API_set_field_double,
    SPVM_RUNTIME_API_set_field_object,
    SPVM_RUNTIME_API_get_package_var_byte,
    SPVM_RUNTIME_API_get_package_var_short,
    SPVM_RUNTIME_API_get_package_var_int,
    SPVM_RUNTIME_API_get_package_var_long,
    SPVM_RUNTIME_API_get_package_var_float,
    SPVM_RUNTIME_API_get_package_var_double,
    SPVM_RUNTIME_API_get_package_var_object,
    SPVM_RUNTIME_API_set_package_var_byte,
    SPVM_RUNTIME_API_set_package_var_short,
    SPVM_RUNTIME_API_set_package_var_int,
    SPVM_RUNTIME_API_set_package_var_long,
    SPVM_RUNTIME_API_set_package_var_float,
    SPVM_RUNTIME_API_set_package_var_double,
    SPVM_RUNTIME_API_set_package_var_object,
    SPVM_RUNTIME_API_pointer,
    SPVM_RUNTIME_API_set_pointer,
    SPVM_RUNTIME_API_call_sub,
    SPVM_RUNTIME_API_exception,
    SPVM_RUNTIME_API_set_exception,
    SPVM_RUNTIME_API_ref_count,
    SPVM_RUNTIME_API_inc_ref_count,
    SPVM_RUNTIME_API_dec_ref_count,
    SPVM_RUNTIME_API_enter_scope,
    SPVM_RUNTIME_API_push_mortal,
    SPVM_RUNTIME_API_leave_scope,
    SPVM_RUNTIME_API_remove_mortal,
    SPVM_RUNTIME_API_is_type,
    SPVM_RUNTIME_API_has_callback,
    SPVM_RUNTIME_API_object_basic_type_id,
    SPVM_RUNTIME_API_object_type_dimension,
    SPVM_RUNTIME_API_weaken,
    SPVM_RUNTIME_API_isweak,
    SPVM_RUNTIME_API_unweaken,
    SPVM_RUNTIME_API_alloc_memory_block_zero,
    SPVM_RUNTIME_API_free_memory_block,
    SPVM_RUNTIME_API_memory_blocks_count,
    SPVM_RUNTIME_API_type_name_raw,
    SPVM_RUNTIME_API_type_name,
    SPVM_RUNTIME_API_new_env,
    SPVM_RUNTIME_API_free_env,
    SPVM_RUNTIME_API_call_begin_blocks,
    SPVM_LIST_new(0), // compiler_module_paths
    SPVM_RUNTIME_API_push_compiler_module_path,
    NULL, // compiler_debug
    SPVM_RUNTIME_API_set_compiler_debug,
  };
  
  int32_t env_length = 255;
  SPVM_ENV* env = SPVM_RUNTIME_API_safe_malloc_zero(sizeof(void*) * env_length);
  memcpy(&env[0], &env_init[0], sizeof(void*) * env_length);

  // Adjust alignment SPVM_VALUE
  int32_t object_header_byte_size = sizeof(SPVM_OBJECT);
  if (object_header_byte_size % sizeof(SPVM_VALUE) != 0) {
    object_header_byte_size += (sizeof(SPVM_VALUE) - object_header_byte_size % sizeof(SPVM_VALUE));
  }
  assert(object_header_byte_size % sizeof(SPVM_VALUE) == 0);
  
  // Object header byte size
  env->object_header_byte_size = (void*)(intptr_t)object_header_byte_size;

  return env;
}
