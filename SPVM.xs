#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "spvm_compiler.h"
#include "spvm_hash.h"
#include "spvm_dynamic_array.h"
#include "spvm_util_allocator.h"
#include "spvm_constant_pool.h"
#include "spvm_bytecode_array.h"
#include "spvm_runtime.h"
#include "spvm_op.h"
#include "spvm_sub.h"
#include "spvm_package.h"
#include "spvm_sub.h"
#include "spvm_my_var.h"
#include "spvm_type.h"
#include "spvm_field.h"
#include "spvm_constant_pool_sub.h"
#include "spvm_constant_pool_package.h"
#include "spvm_constant_pool_field.h"
#include "spvm_constant_pool_type.h"
#include "spvm_global.h"

#include "spvm_api.h"
#include "spvm_xs_util.h"

#include "spvm_extention.h"

static SPVM_API_VALUE call_sub_args[255];

MODULE = SPVM::BaseObject		PACKAGE = SPVM::BaseObject

SV*
DESTROY(...)
  PPCODE:
{
  SV* sv_object = ST(0);
  
  assert(SvOK(sv_object));
  
  // warn("DESTROY");
  
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* object = SPVM_XS_UTIL_get_object(sv_object);
  
  assert(api->get_ref_count(api, object));
  
  // Decrement reference count
  api->dec_ref_count(api, object);
  
  XSRETURN(0);
}

MODULE = SPVM::Object		PACKAGE = SPVM::Object

SV*
new_object(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_package_name = ST(1);
  
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  if (!SvOK(sv_package_name)) {
    croak("Type must be specified(SPVM::Object::new_object)");
  }
  
  const char* package_name = SvPV_nolen(sv_package_name);
  
  int32_t type_id = api->get_type_id(api, package_name);
  
  SPVM_CONSTANT_POOL_TYPE* constant_pool_type = (SPVM_CONSTANT_POOL_TYPE*)&runtime->constant_pool[type_id];
  
  int32_t type_code = constant_pool_type->code;
  
  if (type_id <= 0) {
    croak("Unkown package \"%s\"(SPVM::Object::new_object", package_name);
  }
  
  // Malloc array
  SPVM_API_OBJECT* object =  api->new_object(api, type_id);
  
  // Increment
  api->inc_ref_count(api, object);

  // New sv object
  SV* sv_object = SPVM_XS_UTIL_new_sv_object(type_code, type_id, object);
  
  XPUSHs(sv_object);
  XSRETURN(1);
}

SV*
set(...)
  PPCODE:
{
  SV* sv_object = ST(0);
  SV* sv_field_name = ST(1);
  SV* sv_value = ST(2);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* object = SPVM_XS_UTIL_get_object(sv_object);

  // Package type id
  int32_t package_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_object);
  
  // Package name
  const char* package_name = SPVM_XS_UTIL_get_type_name(package_type_code);
  
  // Field type
  const char* field_name = SvPV_nolen(sv_field_name);
  int32_t field_type_code = SPVM_XS_UTIL_get_field_type_code(package_name, field_name);
  int32_t field_type_id = SPVM_XS_UTIL_get_field_type_id(package_name, field_name);

  // Field id
  int32_t field_id = api->get_field_id(api, object, field_name);
  if (!field_id) {
    croak("Can't find %s \"%s\" field(SPVM::Object::set)", package_name, field_name);
  }
  
  switch (field_type_code) {
    case SPVM_TYPE_C_CODE_BYTE : {
      int8_t value = (int8_t)SvIV(sv_value);
      api->set_byte_field(api, object, field_id, value);
      break;
    }
    case  SPVM_TYPE_C_CODE_SHORT : {
      int16_t value = (int16_t)SvIV(sv_value);
      api->set_short_field(api, object, field_id, value);
      break;
    }
    case SPVM_TYPE_C_CODE_INT : {
      int32_t value = (int32_t)SvIV(sv_value);
      api->set_int_field(api, object, field_id, value);
      break;
    }
    case SPVM_TYPE_C_CODE_LONG : {
      int64_t value = (int64_t)SvIV(sv_value);
      api->set_long_field(api, object, field_id, value);
      break;
    }
    case SPVM_TYPE_C_CODE_FLOAT : {
      float value = (float)SvNV(sv_value);
      api->set_float_field(api, object, field_id, value);
      break;
    }
    case SPVM_TYPE_C_CODE_DOUBLE : {
      double value = (double)SvNV(sv_value);
      api->set_double_field(api, object, field_id, value);
      break;
    }
    default : {
      if (!sv_derived_from(sv_value, "SPVM::BaseObject")) {
        const char* field_type_name = SPVM_XS_UTIL_get_type_name(field_type_code);
        croak("Can't set numeric value to \"%s\" field", field_type_name);
      }
      int32_t value_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_value);
      if (value_type_code != field_type_code) {
        const char* field_type_name = SPVM_XS_UTIL_get_type_name(field_type_code);
        const char* value_type_name = SPVM_XS_UTIL_get_type_name(value_type_code);
        croak("Can't set \"%s\" value to \"%s\" field", value_type_name, field_type_name);
      }
      
      SPVM_API_OBJECT* set_object = SPVM_XS_UTIL_get_object(sv_value);
      
      api->set_object_field(api, object, field_id, set_object);
    }
  }
  
  XSRETURN(0);
}

SV*
get(...)
  PPCODE:
{
  SV* sv_object = ST(0);
  SV* sv_field_name = ST(1);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* object = SPVM_XS_UTIL_get_object(sv_object);
  
  // Package type id
  int32_t package_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_object);

  // Package name
  const char* package_name = SPVM_XS_UTIL_get_type_name(package_type_code);

  // Field type id
  const char* field_name = SvPV_nolen(sv_field_name);
  int32_t field_type_id = SPVM_XS_UTIL_get_field_type_id(package_name, field_name);
  int32_t field_type_code = SPVM_XS_UTIL_get_field_type_code(package_name, field_name);
  warn("AAAAAAAA %d %d", field_type_id, field_type_code);
  
  // Field id
  int32_t field_id = api->get_field_id(api, object, field_name);
  
  if (!field_id) {
    croak("Can't find %s \"%s\" field(SPVM::Object::set)", package_name, field_name);
  }
  
  switch (field_type_code) {
    case SPVM_TYPE_C_CODE_BYTE : {
      int8_t value = api->get_byte_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSViv(value));
      XPUSHs(sv_value);
      break;
    }
    case SPVM_TYPE_C_CODE_SHORT : {
      int16_t value = api->get_short_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSViv(value));
      XPUSHs(sv_value);
      break;
    }
    case SPVM_TYPE_C_CODE_INT : {
      int32_t value = api->get_int_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSViv(value));
      XPUSHs(sv_value);
      break;
    }
    case SPVM_TYPE_C_CODE_LONG : {
      int64_t value = api->get_long_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSViv(value));
      XPUSHs(sv_value);
      break;
    }
    case SPVM_TYPE_C_CODE_FLOAT : {
      float value = api->get_float_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSVnv(value));
      XPUSHs(sv_value);
      break;
    }
    case SPVM_TYPE_C_CODE_DOUBLE : {
      double value = api->get_double_field(api, object, field_id);
      SV* sv_value = sv_2mortal(newSVnv(value));
      XPUSHs(sv_value);
      break;
    }
    default : {
      SPVM_API_OBJECT* value = api->get_object_field(api, object, field_id);
      
      if (value != NULL) {
        api->inc_ref_count(api, value);
      }
      
      switch (field_type_code) {
        case SPVM_TYPE_C_CODE_BYTE_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_byte_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_SHORT_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_short_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_INT_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_int_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_LONG_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_long_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_FLOAT_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_float_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_DOUBLE_ARRAY : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_double_array(value);
          XPUSHs(sv_array);
          break;
        }
        case SPVM_TYPE_C_CODE_STRING : {
          SV* sv_array = SPVM_XS_UTIL_new_sv_string(value);
          XPUSHs(sv_array);
          break;
        }
        default : {
          const char* field_type_name = SPVM_XS_UTIL_get_type_name(field_type_code);
          
          int32_t field_type_name_length = strlen(field_type_name);
          
          if (field_type_name[field_type_name_length - 1] == ']') {
            SV* sv_array = SPVM_XS_UTIL_new_sv_object_array(field_type_code, value);
            XPUSHs(sv_array);
          }
          else {
            SV* sv_object = SPVM_XS_UTIL_new_sv_object(field_type_code, field_type_id, value);
            XPUSHs(sv_object);
          }
        }
      }
    }
  }
  
  XSRETURN(1);
}

MODULE = SPVM::Array::Byte		PACKAGE = SPVM::Array::Byte

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_byte_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_byte_array = SPVM_XS_UTIL_new_sv_byte_array(array);
  
  XPUSHs(sv_byte_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int8_t* elements = api->get_byte_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (int8_t)SvIV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int8_t* elements = api->get_byte_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSViv(elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::Array::Short		PACKAGE = SPVM::Array::Short

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_short_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_short_array(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int16_t* elements = api->get_short_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (int16_t)SvIV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int16_t* elements = api->get_short_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSViv(elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::Array::Int		PACKAGE = SPVM::Array::Int

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_int_array(api, length);

  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_int_array(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int32_t* elements = api->get_int_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (int32_t)SvIV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int32_t* elements = api->get_int_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSViv(elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::Array::Long		PACKAGE = SPVM::Array::Long

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_long_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_long_array(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int64_t* elements = api->get_long_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (int64_t)SvIV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  int64_t* elements = api->get_long_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSViv(elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::Array::Float		PACKAGE = SPVM::Array::Float

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_float_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_float_array(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  float* elements = api->get_float_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (float)SvNV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  float* elements = api->get_float_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSVnv((NV)elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::Array::Double		PACKAGE = SPVM::Array::Double

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_length = ST(1);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_double_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_double_array(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_nums = ST(1);
  AV* av_nums = (AV*)SvRV(sv_nums);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  double* elements = api->get_double_array_elements(api, array);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV** sv_num_ptr = av_fetch(av_nums, i, 0);
      SV* sv_num = sv_num_ptr ? *sv_num_ptr : &PL_sv_undef;
      elements[i] = (double)SvNV(sv_num);
    }
  }
  
  XSRETURN(0);
}

SV*
get_elements(...)
  PPCODE:
{
  SV* sv_array = ST(0);

  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get content
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = api->get_array_length(api, array);
  
  double* elements = api->get_double_array_elements(api, array);
  
  AV* av_nums = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      SV* sv_num = sv_2mortal(newSVnv((NV)elements[i]));
      av_store(av_nums, i, SvREFCNT_inc(sv_num));
    }
  }
  SV* sv_nums = sv_2mortal(newRV_inc((SV*)av_nums));
  
  XPUSHs(sv_nums);
  XSRETURN(1);
}

MODULE = SPVM::String		PACKAGE = SPVM::String

SV*
new_raw(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_string = ST(1);
  
  int32_t length = (int32_t)sv_len(sv_string);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_byte_array(api, length);
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  const char* string = SvPV_nolen(sv_string);
  int8_t* elements = api->get_byte_array_elements(api, array);
  memcpy(elements, string, length);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_string(array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

MODULE = SPVM::Array::Object		PACKAGE = SPVM::Array::Object

SV*
new(...)
  PPCODE:
{
  SV* sv_class = ST(0);
  SV* sv_type_name = ST(1);
  SV* sv_length = ST(2);
  
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  // Malloc array
  SPVM_API_OBJECT* array =  api->new_object_array(api, 0, length);
  
  // Fix type name(int[] -> int[][]);
  sv_catpv(sv_type_name, "[]");
  
  // Type id
  const char* type_name = SvPV_nolen(sv_type_name);
  
  int32_t type_id = api->get_type_id(api, type_name);
  SPVM_CONSTANT_POOL_TYPE* type = (SPVM_CONSTANT_POOL_TYPE*)&runtime->constant_pool[type_id];
  
  int32_t type_code = type->code;
  
  if (type_code < 0) {
    croak("Unknown type %s. Type must be used in SPVM module at least one(SPVM::Array::Object::new())", type_name);
  }
  if (type_code >= SPVM_TYPE_C_CODE_BYTE && type_code <= SPVM_TYPE_C_CODE_DOUBLE) {
    croak("Type is not object array %s(SPVM::Array::Object::new())", type_name);
  }
  
  // Increment reference count
  api->inc_ref_count(api, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_object_array(type_code, array);
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
set(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_index = ST(1);
  SV* sv_object = ST(2);
  
  // Set API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Get array
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  // Array type id
  int32_t array_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_array);
  
  // Array type name
  const char* array_type_name = SPVM_XS_UTIL_get_type_name(array_type_code);
  
  // Object type id
  int32_t object_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_object);
  
  // Object type name
  const char* object_type_name = SPVM_XS_UTIL_get_type_name(object_type_code);
  
  if (strncmp(array_type_name, object_type_name, strlen(array_type_name - 2)) != 0) {
    croak("Invalid type %s is set to object array %s(SPVM::Array::Object::set())", object_type_name, array_type_name);
  }
  
  // Get object
  SPVM_API_OBJECT* object = SPVM_XS_UTIL_get_object(sv_object);
  
  // Index
  int32_t index = (int32_t)SvIV(sv_index);
  
  api->set_object_array_element(api, array, index, (SPVM_API_OBJECT*)object);
  
  XSRETURN(0);
}

SV*
get(...)
  PPCODE:
{
  SV* sv_array = ST(0);
  SV* sv_index = ST(1);
  
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Runtime
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  // Get array
  SPVM_API_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  // Array type id
  int32_t array_type_code = SPVM_XS_UTIL_get_sv_object_type_code(sv_array);
  
  // Array type name
  const char* array_type_name = SPVM_XS_UTIL_get_type_name(array_type_code);
  
  // Element type name sv
  SV* sv_element_type_name = sv_2mortal(newSVpv(array_type_name, strlen(array_type_name) - 2));
  const char* element_type_name = SvPV_nolen(sv_element_type_name);
  
  // Element type id
  int32_t element_type_id = api->get_type_id(api, element_type_name);
  SPVM_CONSTANT_POOL_TYPE* element_type = (SPVM_CONSTANT_POOL_TYPE*)&runtime->constant_pool[element_type_id];
  int32_t element_type_code = element_type->code;

  // Index
  int32_t index = (int32_t)SvIV(sv_index);
  SPVM_API_OBJECT* base_object = api->get_object_array_element(api, array, index);
  if (base_object != NULL) {
    api->inc_ref_count(api, base_object);
  }
  
  SV* sv_base_object;
  switch (element_type_code) {
    case SPVM_TYPE_C_CODE_BYTE_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_byte_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_SHORT_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_short_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_INT_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_int_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_LONG_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_long_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_FLOAT_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_float_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_DOUBLE_ARRAY :
      sv_base_object = SPVM_XS_UTIL_new_sv_double_array((SPVM_API_OBJECT*)base_object);
      break;
    case SPVM_TYPE_C_CODE_STRING :
      sv_base_object = SPVM_XS_UTIL_new_sv_string((SPVM_API_OBJECT*)base_object);
      break;
    default : {
      if (element_type->dimension > 0) {
        sv_base_object = SPVM_XS_UTIL_new_sv_object_array(element_type_code, (SPVM_API_OBJECT*)base_object);
      }
      else {
        sv_base_object = SPVM_XS_UTIL_new_sv_object(element_type_code, element_type_id, (SPVM_API_OBJECT*)base_object);
      }
    }
  }
  
  XPUSHs(sv_base_object);
  
  XSRETURN(1);
}

MODULE = SPVM::Array		PACKAGE = SPVM::Array


MODULE = SPVM		PACKAGE = SPVM

SV*
get_objects_count(...)
  PPCODE:
{
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  int32_t objects_count = api->get_objects_count(api);
  SV* sv_objects_count = sv_2mortal(newSViv(objects_count));
  
  XPUSHs(sv_objects_count);
  XSRETURN(1);
}

SV*
compile(...)
  PPCODE:
{
  // Create compiler
  SPVM_COMPILER* compiler = SPVM_COMPILER_new();
  
  // Debug model
  HV* hv_env = get_hv("ENV", 0);
  SV** sv_debug_ptr = hv_fetch(hv_env, "SPVM_DEBUG", strlen("SPVM_DEBUG"), 0);
  SV* sv_debug = sv_debug_ptr ? *sv_debug_ptr : &PL_sv_undef;
  if (SvTRUE(sv_debug)) {
    compiler->debug = 1;
  }
  
  // Add package
  AV* av_package_infos = get_av("SPVM::PACKAGE_INFOS", 0);
  int32_t av_package_infos_length = (int32_t)av_len(av_package_infos) + 1;
  {
    int32_t i;
    for (i = 0; i < av_package_infos_length; i++) {
      SV** sv_package_info_ptr = av_fetch(av_package_infos, i, 0);
      SV* sv_package_info = sv_package_info_ptr ? *sv_package_info_ptr : &PL_sv_undef;
      HV* hv_package_info = (HV*)SvRV(sv_package_info);
      
      // Name
      SV** sv_name_ptr = hv_fetch(hv_package_info, "name", strlen("name"), 0);
      SV* sv_name = sv_name_ptr ? *sv_name_ptr : &PL_sv_undef;
      const char* name = SvPV_nolen(sv_name);
      
      // File
      SV** sv_file_ptr = hv_fetch(hv_package_info, "file", strlen("file"), 0);
      SV* sv_file = sv_file_ptr ? *sv_file_ptr : &PL_sv_undef;
      const char* file = SvPV_nolen(sv_file);
      
      // Line
      SV** sv_line_ptr = hv_fetch(hv_package_info, "line", strlen("line"), 0);
      SV* sv_line = sv_line_ptr ? *sv_line_ptr : &PL_sv_undef;
      int32_t line = (int32_t)SvIV(sv_line);
      
      // push package to compiler use stack
      SPVM_OP* op_use_package = SPVM_OP_new_op_use_from_package_name(compiler, name, file, line);
      SPVM_DYNAMIC_ARRAY_push(compiler->op_use_stack, op_use_package);
      SPVM_HASH_insert(compiler->op_use_symtable, name, strlen(name), op_use_package);
    }
  }
  
  // Add include paths
  AV* av_include_paths = get_av("main::INC", 0);;
  int32_t av_include_paths_length = (int32_t)av_len(av_include_paths) + 1;
  {
    int32_t i;
    for (i = 0; i < av_include_paths_length; i++) {
      SV** sv_include_path_ptr = av_fetch(av_include_paths, i, 0);
      SV* sv_include_path = sv_include_path_ptr ? *sv_include_path_ptr : &PL_sv_undef;
      char* include_path = SvPV_nolen(sv_include_path);
      SPVM_DYNAMIC_ARRAY_push(compiler->include_pathes, include_path);
    }
  }
  
  // Compile SPVM
  SPVM_COMPILER_compile(compiler);
  if (compiler->error_count > 0) {
    croak("SPVM compile error %d", compiler->error_count);
  }
  
  // Set compiler
  size_t iv_compiler = PTR2IV(compiler);
  SV* sviv_compiler = sv_2mortal(newSViv(iv_compiler));
  SV* sv_compiler = sv_2mortal(newRV_inc(sviv_compiler));
  sv_setsv(get_sv("SPVM::COMPILER", 0), sv_compiler);
  
  XSRETURN(0);
}

SV*
get_sub_names(...)
  PPCODE:
{
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  int32_t subs_base = runtime->subs_base;
  int32_t subs_length = runtime->subs_length;
  AV* av_sub_names = (AV*)sv_2mortal((SV*)newAV());
  
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < subs_length; sub_index++) {
      int32_t sub_id = runtime->constant_pool[subs_base + sub_index];
      
      SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&runtime->constant_pool[sub_id];
      int32_t sub_name_id = constant_pool_sub->abs_name_id;
      int32_t sub_name_length = runtime->constant_pool[sub_name_id];
      const char* sub_name = (char*)&runtime->constant_pool[sub_name_id + 1];
      
      SV* sv_sub_name = sv_2mortal(newSVpv(sub_name, sub_name_length));
      av_push(av_sub_names, SvREFCNT_inc(sv_sub_name));
    }
  }
  
  SV* sv_sub_names = sv_2mortal(newRV_inc((SV*)av_sub_names));
  
  XPUSHs(sv_sub_names);
  XSRETURN(1);
}

SV*
get_native_sub_names(...)
  PPCODE:
{
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  int32_t subs_base = runtime->subs_base;
  int32_t subs_length = runtime->subs_length;
  AV* av_sub_names = (AV*)sv_2mortal((SV*)newAV());
  
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < subs_length; sub_index++) {
      int32_t sub_id = runtime->constant_pool[subs_base + sub_index];
      
      SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&runtime->constant_pool[sub_id];
      if (constant_pool_sub->is_native) {
        int32_t sub_name_id = constant_pool_sub->abs_name_id;
        int32_t sub_name_length = runtime->constant_pool[sub_name_id];
        const char* sub_name = (char*)&runtime->constant_pool[sub_name_id + 1];
        
        SV* sv_sub_name = sv_2mortal(newSVpv(sub_name, sub_name_length));
        av_push(av_sub_names, SvREFCNT_inc(sv_sub_name));
      }
    }
  }
  
  SV* sv_sub_names = sv_2mortal(newRV_inc((SV*)av_sub_names));
  
  XPUSHs(sv_sub_names);
  XSRETURN(1);
}

SV*
bind_native_sub(...)
  PPCODE:
{
  SV* sv_native_sub_name = ST(0);
  SV* sv_native_address = ST(1);
  
  // API
  SPVM_API* api = SPVM_XS_UTIL_get_api();
  
  // Runtime
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  // Native subroutine name
  const char* native_sub_name = SvPV_nolen(sv_native_sub_name);
  
  // Native address
  IV native_address = SvIV(sv_native_address);
  
  // Sub id
  int32_t sub_id = api->get_sub_id(api, native_sub_name);

  // Set native address
  SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&runtime->constant_pool[sub_id];
  constant_pool_sub->native_address = (void*)native_address;

  XSRETURN(0);
}

SV*
build_type_symtable(...)
  PPCODE:
{
  // Get compiler
  SPVM_COMPILER* compiler = SPVM_XS_UTIL_get_compiler();
  
  // Subroutine information
  HV* hv_type_symtable = get_hv("SPVM::TYPE_SYMTABLE", 0);
  
  // abs_name, arg_types, return_type, id, type_code
  SPVM_DYNAMIC_ARRAY* types = compiler->types;
  {
    int32_t type_code;
    for (type_code = 0; type_code < types->length; type_code++) {
      SPVM_TYPE* type = SPVM_DYNAMIC_ARRAY_fetch(types, type_code);
      
      // Type name
      const char* type_name = type->name;
      int32_t type_code = type->code;
      SV* sv_type_code = sv_2mortal(newSViv(type_code));
      
      HV* hv_type_info = (HV*)sv_2mortal((SV*)newHV());
      hv_store(hv_type_info, "id", strlen("id"), SvREFCNT_inc(sv_type_code), 0);
      hv_store(hv_type_info, "type_code", strlen("type_code"), SvREFCNT_inc(sv_type_code), 0);
      SV* sv_type_info = sv_2mortal(newRV_inc((SV*)hv_type_info));
      
      hv_store(hv_type_symtable, type_name, strlen(type_name), SvREFCNT_inc(sv_type_info), 0);
    }
  }
  
  XSRETURN(0);
}

SV*
build_type_names(...)
  PPCODE:
{
  // Get compiler
  SPVM_COMPILER* compiler = SPVM_XS_UTIL_get_compiler();
  
  // Subroutine information
  AV* av_type_names = get_av("SPVM::TYPE_NAMES", 0);
  
  SPVM_DYNAMIC_ARRAY* types = compiler->types;
  {
    int32_t type_code;
    for (type_code = 0; type_code < types->length; type_code++) {
      SPVM_TYPE* type = SPVM_DYNAMIC_ARRAY_fetch(types, type_code);
      
      const char* type_name = type->name;
      SV* sv_type_name = sv_2mortal(newSVpv(type_name, 0));
      int32_t type_code = type->code;
      
      av_store(av_type_names, type_code, SvREFCNT_inc(sv_type_name));
    }
  }
  
  XSRETURN(0);
}

SV*
build_field_symtable(...)
  PPCODE:
{
  // Get compiler
  SPVM_COMPILER* compiler = SPVM_XS_UTIL_get_compiler();
  
  // Field symbol table
  HV* hv_field_symtable = get_hv("SPVM::FIELD_SYMTABLE", 0);
  
  // name, arg_types, return_type
  SPVM_DYNAMIC_ARRAY* op_packages = compiler->op_packages;
  {
    int32_t package_index;
    for (package_index = 0; package_index < op_packages->length; package_index++) {
      SPVM_OP* op_package = SPVM_DYNAMIC_ARRAY_fetch(op_packages, package_index);
      const char* package_name = op_package->uv.package->op_name->uv.name;
      
      HV* hv_package_info = (HV*)sv_2mortal((SV*)newHV());
      
      SPVM_DYNAMIC_ARRAY* op_fields = op_package->uv.package->op_fields;
      {
        int32_t field_index;
        for (field_index = 0; field_index < op_fields->length; field_index++) {
          SPVM_OP* op_field = SPVM_DYNAMIC_ARRAY_fetch(op_fields, field_index);
          SPVM_FIELD* field = op_field->uv.field;
          const char* field_name = field->op_name->uv.name;
          
          // Field type
          const char* field_type = field->op_type->uv.type->name;
          SV* sv_field_type = sv_2mortal(newSVpv(field_type, 0));
          
          // Field type code
          int32_t field_type_code = field->op_type->uv.type->code;
          SV* sv_field_type_code = sv_2mortal(newSViv(field_type_code));
          
          // Field type id
          int32_t field_type_id = field->op_type->uv.type->id;
          SV* sv_field_type_id = sv_2mortal(newSViv(field_type_id));

          // Field id
          int32_t field_id = field->index;
          SV* sv_field_id = sv_2mortal(newSViv(field_id));
          
          HV* hv_field_info = (HV*)sv_2mortal((SV*)newHV());
          hv_store(hv_field_info, "id", strlen("id"), SvREFCNT_inc(sv_field_id), 0);
          hv_store(hv_field_info, "type_code", strlen("type_code"), SvREFCNT_inc(sv_field_type_code), 0);
          hv_store(hv_field_info, "type_id", strlen("type_id"), SvREFCNT_inc(sv_field_type_id), 0);
          SV* sv_field_info = sv_2mortal(newRV_inc((SV*)hv_field_info));
          
          hv_store(hv_package_info, field_name, strlen(field_name), SvREFCNT_inc(sv_field_info), 0);
        }
      }
      
      SV* sv_package_info = sv_2mortal(newRV_inc((SV*)hv_package_info));
      hv_store(hv_field_symtable, package_name, strlen(package_name), SvREFCNT_inc(sv_package_info), 0);
    }
  }
  
  XSRETURN(0);
}

SV*
build_runtime(...)
  PPCODE:
{
  // Get compiler
  SPVM_COMPILER* compiler = SPVM_XS_UTIL_get_compiler();
  
  // Create run-time
  SPVM_RUNTIME* runtime = SPVM_COMPILER_new_runtime(compiler);
  
  // SPVM API
  SPVM_API* api = runtime->api;
  size_t iv_api = PTR2IV(api);
  SV* sviv_api = sv_2mortal(newSViv(iv_api));
  SV* sv_api = sv_2mortal(newRV_inc(sviv_api));
  sv_setsv(get_sv("SPVM::API", 0), sv_api);
  
  XSRETURN(0);
}

SV*
free_compiler(...)
  PPCODE:
{
  // Get compiler
  SPVM_COMPILER* compiler = SPVM_XS_UTIL_get_compiler();
  
  // Free compiler
  SPVM_COMPILER_free(compiler);
  
  // Set undef to compiler
  sv_setsv(get_sv("SPVM::COMPILER", 0), &PL_sv_undef);
  
  XSRETURN(0);
}

SV*
call_sub(...)
  PPCODE:
{
  SV* sv_sub_abs_name = ST(0);
  
  // Get API
  SV* sv_api = get_sv("SPVM::API", 0);
  SV* sviv_api = SvRV(sv_api);
  size_t iv_api = SvIV(sviv_api);
  SPVM_API* api = INT2PTR(SPVM_API*, iv_api);
  
  const char* sub_abs_name = SvPV_nolen(sv_sub_abs_name);
  int32_t sub_id = api->get_sub_id(api, sub_abs_name);
  
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)api->get_runtime(api);
  
  // Subroutine information
  SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&runtime->constant_pool[sub_id];
  
  // Arguments
  {
    int32_t arg_index;
    int32_t arg_type_codes_base = constant_pool_sub->arg_type_codes_base;
    int32_t arg_type_ids_base = constant_pool_sub->arg_type_ids_base;
    int32_t args_length = constant_pool_sub->args_length;
    // Check argument count
    if (items - 1 != args_length) {
      croak("Argument count is defferent");
    }
    
    for (arg_index = 0; arg_index < args_length; arg_index++) {
      SV* sv_value = ST(arg_index + 1);
      
      int32_t arg_type_code = runtime->constant_pool[arg_type_codes_base + arg_index];
      int32_t arg_type_id = runtime->constant_pool[arg_type_ids_base + arg_index];
      
      if (sv_isobject(sv_value)) {
        SV* sv_base_object = sv_value;
        if (sv_derived_from(sv_base_object, "SPVM::BaseObject")) {
         
          HV* hv_base_object = (HV*)SvRV(sv_base_object);
          
          SV** sv_base_object_type_code_ptr = hv_fetch(hv_base_object, "type_code", strlen("type_code"), 0);
          SV* sv_base_object_type_code = *sv_base_object_type_code_ptr;
          int32_t base_object_type_code = SvIV(sv_base_object_type_code);
          
          if (base_object_type_code != arg_type_code) {
            const char* base_object_type_name = SPVM_XS_UTIL_get_type_name(base_object_type_code);
            const char* arg_type_name = SPVM_XS_UTIL_get_type_name(arg_type_code);
            
            croak("Argument base_object type need %s, but %s", arg_type_name, base_object_type_name);
          }
          
          // Get content
          SV** sv_content_ptr = hv_fetch(hv_base_object, "content", strlen("content"), 0);
          SV* sv_content = sv_content_ptr ? *sv_content_ptr : &PL_sv_undef;
          SV* sviv_content = SvRV(sv_content);
          size_t iv_content = SvIV(sviv_content);
          SPVM_API_OBJECT* base_object = INT2PTR(SPVM_API_OBJECT*, iv_content);
          
          call_sub_args[arg_index].object_value = base_object;
        }
        else {
          croak("Object must be derived from SPVM::BaseObject");
        }
      }
      else {
        switch (arg_type_code) {
          case SPVM_TYPE_C_CODE_BYTE : {
            int8_t value = (int8_t)SvIV(sv_value);
            call_sub_args[arg_index].byte_value = value;
            break;
          }
          case  SPVM_TYPE_C_CODE_SHORT : {
            int16_t value = (int16_t)SvIV(sv_value);
            call_sub_args[arg_index].short_value = value;
            break;
          }
          case  SPVM_TYPE_C_CODE_INT : {
            int32_t value = (int32_t)SvIV(sv_value);
            call_sub_args[arg_index].int_value = value;
            break;
          }
          case  SPVM_TYPE_C_CODE_LONG : {
            int64_t value = (int64_t)SvIV(sv_value);
            call_sub_args[arg_index].long_value = value;
            break;
          }
          case  SPVM_TYPE_C_CODE_FLOAT : {
            float value = (float)SvNV(sv_value);
            call_sub_args[arg_index].float_value = value;
            break;
          }
          case  SPVM_TYPE_C_CODE_DOUBLE : {
            double value = (double)SvNV(sv_value);
            call_sub_args[arg_index].double_value = value;
            break;
          }
          default :
            assert(0);
        }
      }
    }
  }
  
  api->set_exception(api, NULL);
  
  // Return type id
  int32_t return_type_id = constant_pool_sub->return_type_id;
  SPVM_CONSTANT_POOL_TYPE* return_type = (SPVM_CONSTANT_POOL_TYPE*)&runtime->constant_pool[return_type_id];
  int32_t return_type_code = return_type->code;
  
  switch (return_type_code) {
    case SPVM_TYPE_C_CODE_VOID:  {
      api->call_void_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      XSRETURN(0);
      break;
    }
    case SPVM_TYPE_C_CODE_BYTE: {
      int8_t return_value = api->call_byte_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSViv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    case SPVM_TYPE_C_CODE_SHORT: {
      int16_t return_value = api->call_short_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSViv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    case SPVM_TYPE_C_CODE_INT: {
      int32_t return_value = api->call_int_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSViv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    case SPVM_TYPE_C_CODE_LONG: {
      int64_t return_value = api->call_long_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSViv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    case SPVM_TYPE_C_CODE_FLOAT: {
      float return_value = api->call_float_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSVnv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    case SPVM_TYPE_C_CODE_DOUBLE: {
      double return_value = api->call_double_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      SV* sv_return_value = sv_2mortal(newSVnv(return_value));
      XPUSHs(sv_return_value);
      XSRETURN(1);
      break;
    }
    default: {
      SPVM_API_OBJECT* return_value = api->call_object_sub(api, sub_id, call_sub_args);
      SPVM_API_OBJECT* exception = api->get_exception(api);
      if (exception) {
        int32_t length = api->get_array_length(api, exception);
        int8_t* exception_bytes = api->get_byte_array_elements(api, exception);
        SV* sv_exception = newSVpv(exception_bytes, length);
        croak("%s", SvPV_nolen(sv_exception));
      }
      
      SV* sv_return_value = NULL;
      if (return_value != NULL) {
        api->inc_ref_count(api, return_value);
        
        switch(return_type_code) {
          case SPVM_TYPE_C_CODE_BYTE_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_byte_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_SHORT_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_short_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_INT_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_int_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_LONG_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_long_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_FLOAT_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_float_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_DOUBLE_ARRAY :
            sv_return_value = SPVM_XS_UTIL_new_sv_double_array((SPVM_API_OBJECT*)return_value);
            break;
          case SPVM_TYPE_C_CODE_STRING :
            sv_return_value = SPVM_XS_UTIL_new_sv_string((SPVM_API_OBJECT*)return_value);
            break;
          default : {
            if (return_type->dimension > 0) {
              sv_return_value = SPVM_XS_UTIL_new_sv_object_array(return_type_code, (SPVM_API_OBJECT*)return_value);
            }
            else {
              sv_return_value = SPVM_XS_UTIL_new_sv_object(return_type_code, return_type_id, (SPVM_API_OBJECT*)return_value);
            }
          }
        }
      }
      else {
        sv_return_value = &PL_sv_undef;
      }
      XPUSHs(sv_return_value);
      XSRETURN(1);
    }
  }
}
