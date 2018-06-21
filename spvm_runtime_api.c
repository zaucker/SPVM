#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>

#include "spvm_runtime_api.h"
#include "spvm_object.h"
#include "spvm_runtime.h"
#include "spvm_native.h"
#include "spvm_global.h"
#include "spvm_type.h"
#include "spvm_hash.h"
#include "spvm_util_allocator.h"
#include "spvm_runtime_allocator.h"
#include "spvm_package.h"
#include "spvm_sub.h"
#include "spvm_package.h"
#include "spvm_type.h"
#include "spvm_field.h"
#include "spvm_compiler.h"
#include "spvm_my.h"
#include "spvm_op.h"
#include "spvm_list.h"
#include "spvm_op_checker.h"
#include "spvm_basic_type.h"








static const void* SPVM_ENV_RUNTIME[]  = {
  SPVM_RUNTIME_API_get_array_length,
  SPVM_RUNTIME_API_get_byte_array_elements,
  SPVM_RUNTIME_API_get_short_array_elements,
  SPVM_RUNTIME_API_get_int_array_elements,
  SPVM_RUNTIME_API_get_long_array_elements,
  SPVM_RUNTIME_API_get_float_array_elements,
  SPVM_RUNTIME_API_get_double_array_elements,
  SPVM_RUNTIME_API_get_object_array_element,
  SPVM_RUNTIME_API_set_object_array_element,
  SPVM_RUNTIME_API_get_field_rel_id,
  SPVM_RUNTIME_API_get_byte_field,
  SPVM_RUNTIME_API_get_short_field,
  SPVM_RUNTIME_API_get_int_field,
  SPVM_RUNTIME_API_get_long_field,
  SPVM_RUNTIME_API_get_float_field,
  SPVM_RUNTIME_API_get_double_field,
  SPVM_RUNTIME_API_get_object_field,
  SPVM_RUNTIME_API_get_struct,
  SPVM_RUNTIME_API_set_byte_field,
  SPVM_RUNTIME_API_set_short_field,
  SPVM_RUNTIME_API_set_int_field,
  SPVM_RUNTIME_API_set_long_field,
  SPVM_RUNTIME_API_set_float_field,
  SPVM_RUNTIME_API_set_double_field,
  SPVM_RUNTIME_API_set_object_field,
  SPVM_RUNTIME_API_get_sub_id,
  SPVM_RUNTIME_API_get_sub_id_method_call,
  SPVM_RUNTIME_API_get_basic_type_id,
  SPVM_RUNTIME_API_new_object,
  SPVM_RUNTIME_API_new_byte_array,
  SPVM_RUNTIME_API_new_short_array,
  SPVM_RUNTIME_API_new_int_array,
  SPVM_RUNTIME_API_new_long_array,
  SPVM_RUNTIME_API_new_float_array,
  SPVM_RUNTIME_API_new_double_array,
  SPVM_RUNTIME_API_new_object_array,
  SPVM_RUNTIME_API_new_multi_array,
  SPVM_RUNTIME_API_new_string,
  SPVM_RUNTIME_API_new_struct,
  SPVM_RUNTIME_API_get_exception,
  SPVM_RUNTIME_API_set_exception,
  SPVM_RUNTIME_API_get_ref_count,
  SPVM_RUNTIME_API_inc_ref_count,
  SPVM_RUNTIME_API_dec_ref_count,
  SPVM_RUNTIME_API_inc_dec_ref_count,
  SPVM_RUNTIME_API_get_objects_count,
  SPVM_RUNTIME_API_get_runtime,
  SPVM_RUNTIME_API_dec_ref_count_only,
  SPVM_RUNTIME_API_weaken,
  SPVM_RUNTIME_API_isweak,
  SPVM_RUNTIME_API_unweaken,
  SPVM_RUNTIME_API_concat,
  SPVM_RUNTIME_API_weaken_object_field,
  SPVM_RUNTIME_API_create_exception_stack_trace,
  SPVM_RUNTIME_API_check_cast,
  (void*)(intptr_t)sizeof(SPVM_OBJECT), // object_header_byte_size
  (void*)(intptr_t)offsetof(SPVM_OBJECT, ref_count), // object_ref_count_byte_offset
  (void*)(intptr_t)offsetof(SPVM_OBJECT, basic_type_id), // object_basic_type_id_byte_offset
  (void*)(intptr_t)offsetof(SPVM_OBJECT, dimension), // object_dimension_byte_offset
  (void*)(intptr_t)offsetof(SPVM_OBJECT, elements_length), // object_elements_length_byte_offset
  SPVM_RUNTIME_call_sub,
};

SPVM_ENV* SPVM_RUNTIME_API_get_env_runtime() {
  return (SPVM_ENV*)SPVM_ENV_RUNTIME;
}

int32_t SPVM_RUNTIME_API_check_cast(SPVM_ENV* env, int32_t cast_basic_type_id, int32_t cast_type_dimension, SPVM_OBJECT* object) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  return SPVM_OP_CHECKER_can_assign(compiler, cast_basic_type_id, cast_type_dimension, object->basic_type_id, object->dimension);
}

SPVM_OBJECT* SPVM_RUNTIME_API_create_exception_stack_trace(SPVM_ENV* env, SPVM_OBJECT* exception, int32_t sub_id, int32_t current_line) {

  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  // Constant pool sub
  SPVM_OP* op_sub = SPVM_LIST_fetch(compiler->op_subs, sub_id);
  SPVM_SUB* sub = op_sub->uv.sub;
  
  // Sub name
  const char* sub_name = sub->abs_name;
  
  // File name
  const char* file_name = sub->file_name;
  
  // stack trace strings
  const char* from = "\n  from ";
  const char* at = "() at ";

  // Exception
  int8_t* exception_bytes = env->get_byte_array_elements(env, exception);
  int32_t exception_length = env->get_array_length(env, exception);
  
  // Total string length
  int32_t total_length = 0;
  total_length += exception_length;
  total_length += strlen(from);
  total_length += strlen(sub_name);
  total_length += strlen(at);
  total_length += strlen(file_name);

  const char* line = " line ";
  char line_str[20];
  
  sprintf(line_str, "%" PRId32, current_line);
  total_length += strlen(line);
  total_length += strlen(line_str);
  
  // Create exception message
  void* new_exception = env->new_string(env, NULL, total_length);
  int8_t* new_exception_bytes = env->get_byte_array_elements(env, new_exception);
  
  memcpy(
    (void*)(new_exception_bytes),
    (void*)(exception_bytes),
    exception_length
  );

  sprintf(
    (char*)new_exception_bytes + exception_length,
    "%s%s%s%s%s%" PRId32,
    from,
    sub_name,
    at,
    file_name,
    line,
    current_line
  );
  
  return new_exception;
}

void SPVM_RUNTIME_API_print(SPVM_ENV* env, SPVM_OBJECT* string) {
  (void)env;
  
  int8_t* bytes = env->get_byte_array_elements(env, string);
  int32_t string_length = env->get_array_length(env, string);
  
  {
    int32_t i;
    for (i = 0; i < string_length; i++) {
      putchar((char)bytes[i]);
    }
  }
}

SPVM_OBJECT* SPVM_RUNTIME_API_concat(SPVM_ENV* env, SPVM_OBJECT* string1, SPVM_OBJECT* string2) {
  (void)env;

  if (string1 == NULL) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, ". operater left string must be defined(string . string)", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return NULL;
  }
  else if (string2 == NULL) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, ". operater right string must be defined(string . string)", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return NULL;
  }
  
  int32_t string1_length = SPVM_RUNTIME_API_get_array_length(env, string1);
  int32_t string2_length = SPVM_RUNTIME_API_get_array_length(env, string2);
  
  int32_t string3_length = string1_length + string2_length;
  SPVM_OBJECT* string3 = SPVM_RUNTIME_API_new_string(env, NULL, string3_length);
  
  int8_t* string1_bytes = SPVM_RUNTIME_API_get_byte_array_elements(env, string1);
  int8_t* string2_bytes = SPVM_RUNTIME_API_get_byte_array_elements(env, string2);
  int8_t* string3_bytes = SPVM_RUNTIME_API_get_byte_array_elements(env, string3);
  
  memcpy(string3_bytes, string1_bytes, string1_length);
  memcpy(string3_bytes + string1_length, string2_bytes, string2_length);
  
  return string3;
}

SPVM_RUNTIME* SPVM_RUNTIME_API_get_runtime() {
  
  return SPVM_GLOBAL_RUNTIME;
}

void SPVM_RUNTIME_API_set_runtime(SPVM_ENV* env, SPVM_RUNTIME* runtime) {
  (void)env;
  
  SPVM_GLOBAL_RUNTIME = runtime;
}

int32_t SPVM_RUNTIME_API_get_objects_count(SPVM_ENV* env) {
  (void)env;
  return SPVM_RUNTIME_API_get_runtime()->objects_count;
}

void SPVM_RUNTIME_API_free_weaken_back_refs(SPVM_ENV* env, void** weaken_back_refs, int32_t weaken_back_refs_length) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  
  {
    int32_t i;
    for (i = 0; i < weaken_back_refs_length; i++) {
      *(void**)weaken_back_refs[i] = NULL;
    }
  }
  
  SPVM_RUNTIME_ALLOCATOR_free(runtime, weaken_back_refs);
}

void SPVM_RUNTIME_API_weaken(SPVM_ENV* env, SPVM_OBJECT** object_address) {
  (void)env;

  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  
  if (*object_address == NULL) {
    return;
  }
  
  if (SPVM_RUNTIME_API_isweak(env, *object_address)) {
    return;
  }
  
  SPVM_OBJECT* object = *object_address;
  
  // Decrelement reference count
  if (object->ref_count == 1) {
    // If reference count is 1, the object is freeed without weaken
    SPVM_RUNTIME_API_dec_ref_count(env, *object_address);
    *object_address = NULL;
    return;
  }
  else {
    object->ref_count--;
  }
  
  // Weaken is implemented tag pointer. If pointer first bit is 1, object is weaken.
  *object_address = (SPVM_OBJECT*)((intptr_t)*object_address | 1);
  
  // Create array of weaken_back_refs if need
  if (object->weaken_back_refs == NULL) {
    object->weaken_back_refs_capacity = 1;
    object->weaken_back_refs = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, sizeof(SPVM_OBJECT*) * object->weaken_back_refs_capacity);
  }
  
  int32_t capacity = object->weaken_back_refs_capacity;
  int32_t length = object->weaken_back_refs_length;
  
  // Extend capacity
  assert(capacity >= length);
  if (length == capacity) {
    int32_t new_capacity = capacity * 2;
    void** new_weaken_back_refs = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, sizeof(SPVM_OBJECT*) * new_capacity);
    
    void** weaken_back_refs = object->weaken_back_refs;
    memcpy(new_weaken_back_refs, weaken_back_refs, length * sizeof(void*));
    
    // Old object become NULL
    memset(weaken_back_refs, 0, length * sizeof(SPVM_OBJECT*));
    
    // Free old weaken back references
    SPVM_RUNTIME_ALLOCATOR_free(runtime, object->weaken_back_refs);
    
    object->weaken_back_refs = new_weaken_back_refs;
    object->weaken_back_refs_capacity = new_capacity;
  }
  
  object->weaken_back_refs[length] = object_address;
  object->weaken_back_refs_length++;
}

int32_t SPVM_RUNTIME_API_isweak(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  int32_t isweak = (intptr_t)object & 1;
  
  return isweak;
}

void SPVM_RUNTIME_API_unweaken(SPVM_ENV* env, SPVM_OBJECT** object_address) {
  (void)env;
  
  if (*object_address == NULL) {
    return;
  }
  
  if (!SPVM_RUNTIME_API_isweak(env, *object_address)) {
    return;
  }
  
  // Unweaken
  *object_address = (SPVM_OBJECT*)((intptr_t)*object_address & ~(intptr_t)1);
  
  SPVM_OBJECT* object = *object_address;
  
  // Increment reference count
  object->ref_count++;

  int32_t length = object->weaken_back_refs_length;
  
  void** weaken_back_refs = object->weaken_back_refs;
  
  {
    int32_t i;
    int32_t found_index = -1;
    for (i = 0; i < length; i++) {
      if (weaken_back_refs[i] == object_address) {
        found_index = i;
        break;
      }
    }
    
    if (found_index == -1) {
      fprintf(stderr, "Not weakened address is specified(SPVM_RUNTIME_API_unweaken())");
      abort();
    }
    if (found_index < length - 1) {
      int32_t move_length = length - found_index - 1;
      memmove(&weaken_back_refs[found_index], &weaken_back_refs[found_index + 1], move_length * sizeof(SPVM_OBJECT*));
    }
  }
  object->weaken_back_refs_length--;
}

void SPVM_RUNTIME_API_set_exception(SPVM_ENV* env, SPVM_OBJECT* exception) {
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  
  if (runtime->exception != NULL) {
    SPVM_RUNTIME_API_dec_ref_count(env, (SPVM_OBJECT*)runtime->exception);
  }
  
  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&runtime->exception, exception);
  
  if (runtime->exception != NULL) {
    runtime->exception->ref_count++;
  }
}

SPVM_OBJECT* SPVM_RUNTIME_API_get_exception(SPVM_ENV* env) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  
  return runtime->exception;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_byte_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(int8_t);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((int8_t*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_BYTE;
  
  // Set array length
  object->elements_length = length;
  
  object->element_byte_size = sizeof(int8_t);
  
  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_short_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(int16_t);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((int16_t*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_SHORT;
  
  // Set array length
  object->elements_length = length;

  object->element_byte_size = sizeof(int16_t);

  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;

  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_int_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(int32_t);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((int32_t*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_INT;

  // Set array length
  object->elements_length = length;

  object->element_byte_size = sizeof(int32_t);

  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_long_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(int64_t);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((int64_t*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_LONG;

  // Set array length
  object->elements_length = length;

  object->element_byte_size = sizeof(int64_t);

  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_float_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(float);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((float*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_FLOAT;

  // Set array length
  object->elements_length = length;

  object->element_byte_size = sizeof(float);

  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_double_array(SPVM_ENV* env, int32_t length) {
  (void)env;
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(double);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((double*)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->dimension = 1;
  object->basic_type_id = SPVM_BASIC_TYPE_C_ID_DOUBLE;
  
  // Set array length
  object->elements_length = length;

  object->element_byte_size = sizeof(double);

  object->category = SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_object_array(SPVM_ENV* env, int32_t basic_type_id, int32_t length) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(SPVM_OBJECT*);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);

  object->basic_type_id = basic_type->id;
  object->dimension = 1;

  // Set array length
  object->elements_length = length;
  
  object->element_byte_size = sizeof(SPVM_OBJECT*);

  object->category = SPVM_OBJECT_C_CATEGORY_OBJECT_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_multi_array(SPVM_ENV* env, int32_t basic_type_id, int32_t element_dimension, int32_t length) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();

  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t array_byte_size = (int64_t)sizeof(SPVM_OBJECT) + ((int64_t)length + 1) * (int64_t)sizeof(SPVM_OBJECT*);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, array_byte_size);
  
  ((SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT)))[length] = 0;
  
  object->basic_type_id = basic_type_id;
  object->dimension = element_dimension + 1;
  
  // Set array length
  object->elements_length = length;
  
  object->element_byte_size = sizeof(SPVM_OBJECT*);

  object->category = SPVM_OBJECT_C_CATEGORY_OBJECT_ARRAY;
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_object(SPVM_ENV* env, int32_t basic_type_id) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);

  SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, basic_type->name, strlen(basic_type->name));
  SPVM_PACKAGE* package = op_package->uv.package;
  
  int32_t fields_length = package->op_fields->length;
  int32_t field_byte_size = sizeof(SPVM_VALUE);
  
  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t object_byte_size = (int64_t)sizeof(SPVM_OBJECT) + (int64_t)(fields_length + 1) * field_byte_size;
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, object_byte_size);
  
  object->basic_type_id = basic_type->id;
  object->dimension = 0;

  object->elements_length = fields_length;
  object->element_byte_size = field_byte_size;

  // Object type id
  object->category = SPVM_OBJECT_C_CATEGORY_OBJECT;
  
  // Has destructor
  if (package->op_sub_destructor) {
    object->has_destructor = 1;
  }
  
  return object;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_struct(SPVM_ENV* env, int32_t basic_type_id, void* struct_ptr) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);

  SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, basic_type->name, strlen(basic_type->name));
  SPVM_PACKAGE* package = op_package->uv.package;
  
  // Alloc length + 1. Last element value is 0 to use c string functions easily
  int64_t object_byte_size = (int64_t)sizeof(SPVM_OBJECT) + (int64_t)sizeof(void*);
  SPVM_OBJECT* object = SPVM_RUNTIME_ALLOCATOR_alloc(runtime, object_byte_size);
  
  *(void**)((intptr_t)object + sizeof(SPVM_OBJECT)) = struct_ptr;
  
  object->basic_type_id = basic_type->id;
  object->dimension = 0;

  object->elements_length = 1;
  object->element_byte_size = sizeof(void*);

  // Object type id
  object->category = SPVM_OBJECT_C_CATEGORY_OBJECT;
  
  // Has destructor
  if (package->op_sub_destructor) {
    object->has_destructor = 1;
  }
  
  return object;
}

int32_t SPVM_RUNTIME_API_get_array_length(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return object->elements_length;
}

SPVM_OBJECT* SPVM_RUNTIME_API_new_string(SPVM_ENV* env, char* bytes, int32_t length) {
  (void)env;

  if (length == 0) {
    length = strlen((char*)bytes);
  }

  SPVM_OBJECT* object = SPVM_RUNTIME_API_new_byte_array(env, length);
  
  if (length > 0) {
    if (bytes == NULL) {
      memset((void*)((intptr_t)object + sizeof(SPVM_OBJECT)), 0, length);
    }
    else {
      memcpy((void*)((intptr_t)object + sizeof(SPVM_OBJECT)), (char*)bytes, length);
    }
  }

  return object;
}

int8_t* SPVM_RUNTIME_API_get_byte_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;

  return (int8_t*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

int16_t* SPVM_RUNTIME_API_get_short_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return (int16_t*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

int32_t* SPVM_RUNTIME_API_get_int_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return (int32_t*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

int64_t* SPVM_RUNTIME_API_get_long_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return (int64_t*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

float* SPVM_RUNTIME_API_get_float_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return (float*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

double* SPVM_RUNTIME_API_get_double_array_elements(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return (double*)((intptr_t)object + sizeof(SPVM_OBJECT));
}

SPVM_OBJECT* SPVM_RUNTIME_API_get_object_array_element(SPVM_ENV* env, SPVM_OBJECT* object, int32_t index) {
  (void)env;
  
  SPVM_OBJECT** values = (SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT));

  assert(object);
  assert(index >= 0);
  assert(index <= object->elements_length);
  
  SPVM_OBJECT* oval = values[index];
  
  return oval;
}

void SPVM_RUNTIME_API_set_object_array_element(SPVM_ENV* env, SPVM_OBJECT* object, int32_t index, SPVM_OBJECT* oval) {
  (void)env;
  
  SPVM_OBJECT** values = (SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT));
  
  assert(object);
  assert(index >= 0);
  assert(index <= object->elements_length);
  
  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&values[index], oval);
}

void* SPVM_RUNTIME_API_get_struct(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  void* struct_ptr = *(void**)((intptr_t)object + sizeof(SPVM_OBJECT));
  
  return struct_ptr;
}

void SPVM_RUNTIME_API_inc_dec_ref_count(SPVM_ENV* env, SPVM_OBJECT* object) {
  SPVM_RUNTIME_API_inc_ref_count(env, object);
  SPVM_RUNTIME_API_dec_ref_count(env, object);
}

void SPVM_RUNTIME_API_dec_ref_count_only(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  if (object != NULL) {
    assert(object->ref_count > 0);
    // Increment reference count
    object->ref_count--;
  }
}

void SPVM_RUNTIME_API_dec_ref_count(SPVM_ENV* env, SPVM_OBJECT* object) {
  
  assert(object != NULL);
  assert(object->ref_count > 0);
  
  if (__builtin_expect(object->ref_count < 1, 0)) {
    fprintf(stderr, "Found invalid reference count object(SPVM_RUNTIME_API_dec_ref_count())");
    abort();
  }
  
  // Decrement reference count
  object->ref_count--;
  
  // If reference count is zero, free address.
  if (object->ref_count == 0) {
    SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
    SPVM_COMPILER* compiler = runtime->compiler;
    
    if (__builtin_expect(object->has_destructor, 0)) {
      if (object->in_destroy) {
        return;
      }
      else {
        SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, object->basic_type_id);
        SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, basic_type->name, strlen(basic_type->name));
        SPVM_PACKAGE* package = op_package->uv.package;
        
        // Call destructor
        SPVM_VALUE args[1];
        args[0].oval = object;
        object->in_destroy = 1;
        SPVM_RUNTIME_call_sub(env, package->op_sub_destructor->uv.sub->id, args);
        object->in_destroy = 0;
        
        if (object->ref_count < 0) {
          printf("object reference count become minus in DESTROY()\n");
          abort();
        }
      }
    }
    
    if (object->category == SPVM_OBJECT_C_CATEGORY_OBJECT_ARRAY) {
      int32_t length = object->elements_length;
      {
        int32_t i;
        for (i = 0; i < length; i++) {
          SPVM_OBJECT** object_field_address = (SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT) + sizeof(SPVM_OBJECT*) * i);
          if (*object_field_address != NULL) {
            SPVM_RUNTIME_API_dec_ref_count(env, *object_field_address);
          }
        }
      }
    }
    else if (object->category == SPVM_OBJECT_C_CATEGORY_OBJECT) {
      
      // Type
      SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, object->basic_type_id);
      SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, basic_type->name, strlen(basic_type->name));
      SPVM_PACKAGE* package = op_package->uv.package;
      
      {
        int32_t index;
        for (index = 0; index < package->object_field_rel_ids->length; index++) {
          int32_t object_field_rel_id = (intptr_t)SPVM_LIST_fetch(package->object_field_rel_ids, index);
          SPVM_OBJECT** object_field_address = (SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT) + sizeof(SPVM_VALUE) * object_field_rel_id);
          if (*object_field_address != NULL) {
            // If object is weak, unweaken
            if (__builtin_expect(SPVM_RUNTIME_API_isweak(env, *object_field_address), 0)) {
              SPVM_RUNTIME_API_unweaken(env, object_field_address);
            }
            else {
              SPVM_RUNTIME_API_dec_ref_count(env, *object_field_address);
            }
          }
        }
      }
      if (__builtin_expect(object->weaken_back_refs != NULL, 0)) {
        SPVM_RUNTIME_API_free_weaken_back_refs(env, object->weaken_back_refs, object->weaken_back_refs_length);
      }
    }
    
    SPVM_RUNTIME_ALLOCATOR_free(runtime, object);
  }
}

void SPVM_RUNTIME_API_inc_ref_count(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  if (object != NULL) {
    assert(object->ref_count >= 0);
    // Increment reference count
    object->ref_count++;
  }
}

int32_t SPVM_RUNTIME_API_get_ref_count(SPVM_ENV* env, SPVM_OBJECT* object) {
  (void)env;
  
  return object->ref_count;
}

int32_t SPVM_RUNTIME_API_get_field_rel_id(SPVM_ENV* env, const char* package_name, const char* field_name) {
  (void)env;
  
  // Runtime
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  // Package
  SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, package_name, strlen(package_name));
  
  // Field
  SPVM_HASH* op_field_symtable = op_package->uv.package->op_field_symtable;
  SPVM_OP* op_field = SPVM_HASH_fetch(op_field_symtable, field_name, strlen(field_name));
  
  int32_t field_rel_id;
  if (op_field) {
    field_rel_id = op_field->uv.field->rel_id;
  }
  else {
    field_rel_id = -1;
  }
  
  return field_rel_id;
}

int32_t SPVM_RUNTIME_API_get_sub_id(SPVM_ENV* env, const char* name) {
  (void)env;
  
  if (name == NULL) {
    return 0;
  }
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SPVM_OP* op_sub = SPVM_HASH_fetch(compiler->op_sub_symtable, name, strlen(name));
  int32_t sub_id = op_sub->uv.sub->id;
  
  return sub_id;
}

int32_t SPVM_RUNTIME_API_get_sub_id_method_call(SPVM_ENV* env, SPVM_OBJECT* object, const char* sub_name) {
  (void)env;
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, object->basic_type_id);
  SPVM_OP* op_package = SPVM_HASH_fetch(compiler->op_package_symtable, basic_type->name, strlen(basic_type->name));  
  SPVM_PACKAGE* package = op_package->uv.package;
  
  SPVM_OP* op_sub = SPVM_HASH_fetch(package->op_sub_symtable, sub_name, strlen(sub_name));
  
  return  op_sub->uv.sub->id;
}

int32_t SPVM_RUNTIME_API_get_basic_type_id(SPVM_ENV* env, const char* name) {
  (void)env;
  
  if (name == NULL) {
    return 0;
  }
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SPVM_BASIC_TYPE* basic_type = SPVM_HASH_fetch(compiler->basic_type_symtable, name, strlen(name));
  if (basic_type) {
    int32_t basic_type_id = basic_type->id;
    return basic_type_id;
  }
  else {
    return -1;
  }
}

int8_t SPVM_RUNTIME_API_get_byte_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_byte_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }
  
  int8_t value = *(int8_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

int16_t SPVM_RUNTIME_API_get_short_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_short_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }

  int16_t value = *(int16_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

int32_t SPVM_RUNTIME_API_get_int_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_int_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }

  int32_t value = *(int32_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}
int64_t SPVM_RUNTIME_API_get_long_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_long_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }

  int64_t value = *(int64_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

float SPVM_RUNTIME_API_get_float_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_float_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }

  float value = *(float*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

double SPVM_RUNTIME_API_get_double_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_double_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return 0;
  }

  double value = *(double*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

void SPVM_RUNTIME_API_weaken_object_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(!object, 0)) {
    SPVM_OBJECT* exception = env->new_string(env, "Object to weaken an object field must not be undefined.", 0);
    env->set_exception(env, exception);
    return;
  }

  SPVM_OBJECT** object_address = (SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT) + sizeof(SPVM_VALUE) * field_rel_id);
  
  // Weaken object field
  if (*object_address != NULL) {
    SPVM_RUNTIME_API_weaken(env, object_address);
  }
  
  return;
}

SPVM_OBJECT* SPVM_RUNTIME_API_get_object_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(get_object_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return NULL;
  }

  void* value = *(void**)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id);
  
  return value;
}

void SPVM_RUNTIME_API_set_byte_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, int8_t value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_byte_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }

  *(int8_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_short_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, int16_t value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_short_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }

  *(int16_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_int_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, int32_t value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_int_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }

  *(int32_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_long_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, int64_t value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_long_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }

  *(int64_t*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_float_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, float value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_float_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }
  
  *(float*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_double_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, double value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_double_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }

  *(double*)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id) = value;
}

void SPVM_RUNTIME_API_set_object_field(SPVM_ENV* env, SPVM_OBJECT* object, int32_t field_rel_id, SPVM_OBJECT* value) {

  if (__builtin_expect(object == NULL, 0)) {
    SPVM_OBJECT* exception = SPVM_RUNTIME_API_new_string(env, "Object must be not undef(set_object_field).", 0);
    SPVM_RUNTIME_API_set_exception(env, exception);
    return;
  }
  
  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN((SPVM_OBJECT**)((intptr_t)object + sizeof(SPVM_OBJECT) + field_rel_id), value);
}
