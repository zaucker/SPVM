#ifndef SPVM_RUNTIME_API_H
#define SPVM_RUNTIME_API_H

#include "spvm_base.h"
#include "spvm_api.h"

// Array functions
int32_t SPVM_RUNTIME_API_get_array_length(SPVM_API* api, SPVM_ARRAY* array);
int8_t* SPVM_RUNTIME_API_get_byte_array_elements(SPVM_API* api, SPVM_ARRAY* array);
int16_t* SPVM_RUNTIME_API_get_short_array_elements(SPVM_API* api, SPVM_ARRAY* array);
int32_t* SPVM_RUNTIME_API_get_int_array_elements(SPVM_API* api, SPVM_ARRAY* array);
int64_t* SPVM_RUNTIME_API_get_long_array_elements(SPVM_API* api, SPVM_ARRAY* array);
float* SPVM_RUNTIME_API_get_float_array_elements(SPVM_API* api, SPVM_ARRAY* array);
double* SPVM_RUNTIME_API_get_double_array_elements(SPVM_API* api, SPVM_ARRAY* array);
void SPVM_RUNTIME_API_set_object_array_element(SPVM_API* api, SPVM_ARRAY* array, int32_t index, SPVM_BASE_OBJECT* value);
SPVM_BASE_OBJECT* SPVM_RUNTIME_API_get_object_array_element(SPVM_API* api, SPVM_ARRAY* array, int32_t index);

// Object functions
int32_t SPVM_RUNTIME_API_get_field_id(SPVM_API* api, SPVM_OBJECT* object, const char* name);
int8_t SPVM_RUNTIME_API_get_byte_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
int16_t SPVM_RUNTIME_API_get_short_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
int32_t SPVM_RUNTIME_API_get_int_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
int64_t SPVM_RUNTIME_API_get_long_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
float SPVM_RUNTIME_API_get_float_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
double SPVM_RUNTIME_API_get_double_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
SPVM_BASE_OBJECT* SPVM_RUNTIME_API_get_object_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id);
void SPVM_RUNTIME_API_set_byte_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, int8_t value);
void SPVM_RUNTIME_API_set_short_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, int16_t value);
void SPVM_RUNTIME_API_set_int_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, int32_t value);
void SPVM_RUNTIME_API_set_long_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, int64_t value);
void SPVM_RUNTIME_API_set_float_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, float value);
void SPVM_RUNTIME_API_set_double_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, double value);
void SPVM_RUNTIME_API_set_object_field(SPVM_API* api, SPVM_OBJECT* object, int32_t field_id, SPVM_BASE_OBJECT* value);

// Call Subroutine
void SPVM_RUNTIME_API_call_void_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
int8_t SPVM_RUNTIME_API_call_byte_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
int16_t SPVM_RUNTIME_API_call_short_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
int32_t SPVM_RUNTIME_API_call_int_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
int64_t SPVM_RUNTIME_API_call_long_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
float SPVM_RUNTIME_API_call_float_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
double SPVM_RUNTIME_API_call_double_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);
SPVM_BASE_OBJECT* SPVM_RUNTIME_API_call_object_sub(SPVM_API* api, int32_t sub_constant_pool_index, SPVM_VALUE* args);

// Internal functions
int64_t SPVM_RUNTIME_API_calcurate_base_object_byte_size(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
SPVM_ARRAY* SPVM_RUNTIME_API_new_byte_array_from_pv(SPVM_API* api, const char* pv);
SPVM_VALUE* SPVM_RUNTIME_API_get_fields(SPVM_API* api, SPVM_OBJECT* object);
int32_t SPVM_RUNTIME_API_get_fields_length(SPVM_API* api, SPVM_OBJECT* object);
int32_t SPVM_RUNTIME_API_dump_field_names(SPVM_API* api, SPVM_OBJECT* object);
int32_t SPVM_RUNTIME_API_get_ref_count(SPVM_API* api, SPVM_BASE_OBJECT* base_object);

int32_t SPVM_RUNTIME_API_get_sub_id(SPVM_API* api, const char* name);
int32_t SPVM_RUNTIME_API_get_package_id(SPVM_API* api, const char* name);

SPVM_OBJECT* SPVM_RUNTIME_API_new_object(SPVM_API* api, int32_t package_id);

SPVM_ARRAY* SPVM_RUNTIME_API_new_byte_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_short_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_int_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_long_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_float_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_double_array(SPVM_API* api, int32_t length);
SPVM_ARRAY* SPVM_RUNTIME_API_new_object_array(SPVM_API* api, int32_t length);

// Exception
void SPVM_RUNTIME_API_set_exception(SPVM_API* api, SPVM_ARRAY* exception);
SPVM_ARRAY* SPVM_RUNTIME_API_get_exception(SPVM_API* api);

// Reference count
int32_t SPVM_RUNTIME_API_get_ref_count(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
void SPVM_RUNTIME_API_dec_ref_count(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
void SPVM_RUNTIME_API_inc_ref_count(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
void SPVM_RUNTIME_API_inc_dec_ref_count(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
_Bool SPVM_RUNTIME_API_isweak(SPVM_API* api, SPVM_BASE_OBJECT* base_object);
void SPVM_RUNTIME_API_weaken(SPVM_API* api, SPVM_BASE_OBJECT** base_object_address);

#endif
