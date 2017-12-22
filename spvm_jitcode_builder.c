#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include "spvm_compiler.h"
#include "spvm_jitcode_builder.h"
#include "spvm_compiler.h"
#include "spvm_string_buffer.h"
#include "spvm_sub.h"
#include "spvm_op.h"
#include "spvm_dynamic_array.h"
#include "spvm_type.h"
#include "spvm_object.h"
#include "spvm_runtime.h"
#include "spvm_type.h"
#include "spvm_my.h"
#include "spvm_constant_pool.h"
#include "spvm_constant_pool_sub.h"
#include "spvm_constant_pool_package.h"
#include "spvm_constant_pool_field.h"
#include "spvm_constant_pool_type.h"
#include "spvm_opcode.h"
#include "spvm_opcode_array.h"

void SPVM_JITCODE_BUILDER_build_jitcode(SPVM_COMPILER* compiler) {
  (void)compiler;

  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_get_runtime();
  
  SPVM_STRING_BUFFER* string_buffer = SPVM_STRING_BUFFER_new(0);
  
  // C library
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stdint.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <assert.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <string.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <math.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stdlib.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stdio.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <inttypes.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stddef.h>\n");

  // API header
  SPVM_STRING_BUFFER_add(string_buffer, "#include <spvm_api.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  // Constant macro
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_C_OBJECT_HEADER_BYTE_SIZE ");
  SPVM_STRING_BUFFER_add_int(string_buffer, sizeof(SPVM_OBJECT));
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_C_OBJECT_LENGTH_BYTE_OFFSET ");
  SPVM_STRING_BUFFER_add_int(string_buffer, (int32_t)offsetof(SPVM_OBJECT, length));
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_C_OBJECT_REF_COUNT_BYTE_OFFSET ");
  SPVM_STRING_BUFFER_add_int(string_buffer, (int32_t)offsetof(SPVM_OBJECT, ref_count));
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_C_RUNTIME_EXCEPTION_BYTE_OFFSET ");
  SPVM_STRING_BUFFER_add_int(string_buffer, (int32_t)offsetof(SPVM_RUNTIME, exception));
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  // Inline macro function
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_INLINE_INC_REF_COUNT(object) ((*(int32_t*)((intptr_t)object + SPVM_JITCODE_C_OBJECT_REF_COUNT_BYTE_OFFSET))++)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_INLINE_DEC_REF_COUNT_ONLY(object) ((*(int32_t*)((intptr_t)object + SPVM_JITCODE_C_OBJECT_REF_COUNT_BYTE_OFFSET))--)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_INLINE_GET_EXCEPTION() (*(SPVM_API_OBJECT**)((intptr_t)SPVM_JITCODE_C_RUNTIME + SPVM_JITCODE_C_RUNTIME_EXCEPTION_BYTE_OFFSET)))\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_INLINE_SET_EXCEPTION_NULL()\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  do { \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if ((*(SPVM_API_OBJECT**)((intptr_t)SPVM_JITCODE_C_RUNTIME + SPVM_JITCODE_C_RUNTIME_EXCEPTION_BYTE_OFFSET)) != NULL) { \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "      api->dec_ref_count(api, (*(SPVM_API_OBJECT**)((intptr_t)SPVM_JITCODE_C_RUNTIME + SPVM_JITCODE_C_RUNTIME_EXCEPTION_BYTE_OFFSET))); \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    } \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    (*(SPVM_API_OBJECT**)((intptr_t)SPVM_JITCODE_C_RUNTIME + SPVM_JITCODE_C_RUNTIME_EXCEPTION_BYTE_OFFSET)) = NULL; \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  while (0) \\\n");

  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_JITCODE_INLINE_ISWEAK(object) ((intptr_t)object & 1)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  // Constant pool
  int32_t* constant_pool = compiler->constant_pool->values;

  int32_t subs_base = compiler->subs_base;
  int32_t subs_length = compiler->op_subs->length;

  // Subroutine Declaration
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < subs_length; sub_index++) {
      int32_t sub_id = constant_pool[subs_base + sub_index];
      
      SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&constant_pool[sub_id];
      int32_t sub_abs_name_id = constant_pool_sub->abs_name_id;
      int32_t sub_abs_name_length = constant_pool[sub_abs_name_id];
      
      // Subroutine name
      const char* sub_abs_name = (char*)&constant_pool[sub_abs_name_id + 1];

      // Arguments length
      int32_t args_length = constant_pool_sub->args_length;
      
      // Arguments type ids base
      int32_t arg_type_ids_base = constant_pool_sub->arg_type_ids_base;
      
      // Return type code
      int32_t return_type_id = constant_pool_sub->return_type_id;
      SPVM_CONSTANT_POOL_TYPE* return_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[return_type_id];
      int32_t return_type_code = return_type->code;

      // Mys length
      int32_t mys_length = constant_pool_sub->mys_length;

      // My type ids base
      int32_t my_type_ids_base = constant_pool_sub->my_type_ids_base;

      // Return type
      switch (return_type->code) {
        case SPVM_TYPE_C_CODE_VOID:
          SPVM_STRING_BUFFER_add(string_buffer, "void ");
          break;
        case SPVM_TYPE_C_CODE_BYTE:
          SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
          break;
        case SPVM_TYPE_C_CODE_SHORT:
          SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
          break;
        case SPVM_TYPE_C_CODE_INT:
          SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
          break;
        case SPVM_TYPE_C_CODE_LONG:
          SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
          break;
        case SPVM_TYPE_C_CODE_FLOAT:
          SPVM_STRING_BUFFER_add(string_buffer, "float ");
          break;
        case SPVM_TYPE_C_CODE_DOUBLE:
          SPVM_STRING_BUFFER_add(string_buffer, "double ");
          break;
        default:
          SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
      }

      // Subroutine name. Replace : to _
      SPVM_STRING_BUFFER_add(string_buffer, "SPVM_JITCODE_");
      SPVM_STRING_BUFFER_add(string_buffer, (char*)sub_abs_name);
      {
        int32_t index = string_buffer->length - strlen(sub_abs_name);
        
        while (index < string_buffer->length) {
          if (string_buffer->buffer[index] == ':') {
            string_buffer->buffer[index] = '_';
          }
          index++;
        }
      }

      // Arguments
      SPVM_STRING_BUFFER_add(string_buffer, "(");
      {
        int32_t arg_index;
        for (arg_index = 0; arg_index < args_length; arg_index++) {
          int32_t arg_type_id = constant_pool[arg_type_ids_base + arg_index];

          // Argument type code
          SPVM_CONSTANT_POOL_TYPE* constant_pool_arg_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[arg_type_id];
          int32_t arg_type_code = constant_pool_arg_type->code;
          
          switch (arg_type_code) {
            case SPVM_TYPE_C_CODE_BYTE : {
              SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_SHORT : {
              SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_INT : {
              SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_LONG : {
              SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_FLOAT : {
              SPVM_STRING_BUFFER_add(string_buffer, "float ");
              break;
            }
            case  SPVM_TYPE_C_CODE_DOUBLE : {
              SPVM_STRING_BUFFER_add(string_buffer, "double ");
              break;
            }
            default : {
              SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
            }
          }
          SPVM_STRING_BUFFER_add(string_buffer, "arg");
          SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
          if (arg_index != args_length - 1) {
            SPVM_STRING_BUFFER_add(string_buffer, ", ");
          }
        }
      }
      SPVM_STRING_BUFFER_add(string_buffer, ");\n");
    }
    SPVM_STRING_BUFFER_add(string_buffer, "\n");
  }

  // Subroutine Implementations
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < subs_length; sub_index++) {
      int32_t sub_id = constant_pool[subs_base + sub_index];
      
      SPVM_CONSTANT_POOL_SUB* constant_pool_sub = (SPVM_CONSTANT_POOL_SUB*)&constant_pool[sub_id];
      int32_t sub_abs_name_id = constant_pool_sub->abs_name_id;
      int32_t sub_abs_name_length = constant_pool[sub_abs_name_id];
      
      // Subroutine name
      const char* sub_abs_name = (char*)&constant_pool[sub_abs_name_id + 1];

      // Arguments length
      int32_t args_length = constant_pool_sub->args_length;
      
      // Arguments type ids base
      int32_t arg_type_ids_base = constant_pool_sub->arg_type_ids_base;
      
      // Return type code
      int32_t return_type_id = constant_pool_sub->return_type_id;
      SPVM_CONSTANT_POOL_TYPE* return_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[return_type_id];
      int32_t return_type_code = return_type->code;

      // Mys length
      int32_t mys_length = constant_pool_sub->mys_length;

      // My type ids base
      int32_t my_type_ids_base = constant_pool_sub->my_type_ids_base;

      // Return type
      switch (return_type->code) {
        case SPVM_TYPE_C_CODE_VOID:
          SPVM_STRING_BUFFER_add(string_buffer, "void ");
          break;
        case SPVM_TYPE_C_CODE_BYTE:
          SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
          break;
        case SPVM_TYPE_C_CODE_SHORT:
          SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
          break;
        case SPVM_TYPE_C_CODE_INT:
          SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
          break;
        case SPVM_TYPE_C_CODE_LONG:
          SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
          break;
        case SPVM_TYPE_C_CODE_FLOAT:
          SPVM_STRING_BUFFER_add(string_buffer, "float ");
          break;
        case SPVM_TYPE_C_CODE_DOUBLE:
          SPVM_STRING_BUFFER_add(string_buffer, "double ");
          break;
        default:
          SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
      }

      // Subroutine name. Replace : to _
      SPVM_STRING_BUFFER_add(string_buffer, "SPVM_JITCODE_");
      SPVM_STRING_BUFFER_add(string_buffer, (char*)sub_abs_name);
      {
        int32_t index = string_buffer->length - strlen(sub_abs_name);
        
        while (index < string_buffer->length) {
          if (string_buffer->buffer[index] == ':') {
            string_buffer->buffer[index] = '_';
          }
          index++;
        }
      }

      // Arguments
      SPVM_STRING_BUFFER_add(string_buffer, "(");
      {
        int32_t arg_index;
        for (arg_index = 0; arg_index < args_length; arg_index++) {
          int32_t arg_type_id = constant_pool[arg_type_ids_base + arg_index];

          // Argument type code
          SPVM_CONSTANT_POOL_TYPE* constant_pool_arg_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[arg_type_id];
          int32_t arg_type_code = constant_pool_arg_type->code;
          
          switch (arg_type_code) {
            case SPVM_TYPE_C_CODE_BYTE : {
              SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_SHORT : {
              SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_INT : {
              SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_LONG : {
              SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_FLOAT : {
              SPVM_STRING_BUFFER_add(string_buffer, "float ");
              break;
            }
            case  SPVM_TYPE_C_CODE_DOUBLE : {
              SPVM_STRING_BUFFER_add(string_buffer, "double ");
              break;
            }
            default : {
              SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
            }
          }
          SPVM_STRING_BUFFER_add(string_buffer, "arg");
          SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
          if (arg_index != args_length - 1) {
            SPVM_STRING_BUFFER_add(string_buffer, ", ");
          }
        }
      }
      
      // Block start
      SPVM_STRING_BUFFER_add(string_buffer, ") {\n");
      
      // API
      SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API* api = ");
      SPVM_STRING_BUFFER_add_address(string_buffer, runtime->api);
      SPVM_STRING_BUFFER_add(string_buffer, ";\n");
      
      // Lexical variables
      {
        int32_t my_index;
        for (my_index = args_length; my_index < mys_length; my_index++) {
          int32_t my_type_id = constant_pool[my_type_ids_base + my_index];

          // My type code
          SPVM_CONSTANT_POOL_TYPE* constant_pool_my_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[my_type_id];
          int32_t my_type_code = constant_pool_my_type->code;
          
          switch (my_type_code) {
            case SPVM_TYPE_C_CODE_BYTE : {
              SPVM_STRING_BUFFER_add(string_buffer, "  int8_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_SHORT : {
              SPVM_STRING_BUFFER_add(string_buffer, "  int16_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_INT : {
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_LONG : {
              SPVM_STRING_BUFFER_add(string_buffer, "  int64_t ");
              break;
            }
            case  SPVM_TYPE_C_CODE_FLOAT : {
              SPVM_STRING_BUFFER_add(string_buffer, "  float ");
              break;
            }
            case  SPVM_TYPE_C_CODE_DOUBLE : {
              SPVM_STRING_BUFFER_add(string_buffer, "  double ");
              break;
            }
            default : {
              SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* ");
            }
          }
          SPVM_STRING_BUFFER_add(string_buffer, "var");
          SPVM_STRING_BUFFER_add_int(string_buffer, my_index);
          SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        }
      }
      
      // Eval stack
      if (constant_pool_sub->eval_stack_max_length > 0) {
        // Eval stack
        SPVM_STRING_BUFFER_add(string_buffer, "  int32_t eval_stack[");
        SPVM_STRING_BUFFER_add_int(string_buffer, constant_pool_sub->eval_stack_max_length);
        SPVM_STRING_BUFFER_add(string_buffer, "];\n");
        
        // Eval stack top
        SPVM_STRING_BUFFER_add(string_buffer, "  int32_t eval_stack_top = -1;\n");
      }

      // If arg is object, increment reference count
      {
        int32_t arg_index;
        for (arg_index = 0; arg_index < args_length; arg_index++) {
          int32_t arg_type_id = constant_pool[arg_type_ids_base + arg_index];

          // Argument type code
          SPVM_CONSTANT_POOL_TYPE* constant_pool_arg_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[arg_type_id];
          
          if (!constant_pool_arg_type->is_numeric) {
            SPVM_STRING_BUFFER_add(string_buffer, "  if (var");
            SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
            SPVM_STRING_BUFFER_add(string_buffer, " != NULL) {\n");
            
            SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_JITCODE_INLINE_INC_REF_COUNT(var");
            SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            
            SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
          }
        }
        SPVM_STRING_BUFFER_add(string_buffer, "\n");
      }

      // Set exception to NULL
      SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_JITCODE_INLINE_SET_EXCEPTION_NULL();\n");
      SPVM_STRING_BUFFER_add(string_buffer, "\n");
      
      // Native subroutine
      if (constant_pool_sub->is_native) {
        // Assign native address
        switch (return_type_code) {
          case SPVM_TYPE_C_CODE_VOID:
            SPVM_STRING_BUFFER_add(string_buffer, "  void ");
            break;
          case SPVM_TYPE_C_CODE_BYTE:
            SPVM_STRING_BUFFER_add(string_buffer, "  int8_t ");
            break;
          case SPVM_TYPE_C_CODE_SHORT:
            SPVM_STRING_BUFFER_add(string_buffer, "  int16_t ");
            break;
          case SPVM_TYPE_C_CODE_INT:
            SPVM_STRING_BUFFER_add(string_buffer, "  int32_t ");
            break;
          case SPVM_TYPE_C_CODE_LONG:
            SPVM_STRING_BUFFER_add(string_buffer, "  int64_t ");
            break;
          case SPVM_TYPE_C_CODE_FLOAT:
            SPVM_STRING_BUFFER_add(string_buffer, "  float ");
            break;
          case SPVM_TYPE_C_CODE_DOUBLE:
            SPVM_STRING_BUFFER_add(string_buffer, "  double ");
            break;
          default:
            SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* ");
        }
        if (args_length > 0) {
          SPVM_STRING_BUFFER_add(string_buffer, "(*native_address)(SPVM_API*, ");
        }
        else {
          SPVM_STRING_BUFFER_add(string_buffer, "(*native_address)(SPVM_API*");
        }
        {
          int32_t arg_index;
          for (arg_index = 0; arg_index < args_length; arg_index++) {
            int32_t arg_type_id = constant_pool[arg_type_ids_base + arg_index];

            // Argument type code
            SPVM_CONSTANT_POOL_TYPE* constant_pool_arg_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[arg_type_id];
            int32_t arg_type_code = constant_pool_arg_type->code;
            
            switch (arg_type_code) {
              case SPVM_TYPE_C_CODE_BYTE : {
                SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_SHORT : {
                SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_INT : {
                SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_LONG : {
                SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_FLOAT : {
                SPVM_STRING_BUFFER_add(string_buffer, "float ");
                break;
              }
              case  SPVM_TYPE_C_CODE_DOUBLE : {
                SPVM_STRING_BUFFER_add(string_buffer, "double ");
                break;
              }
              default : {
                SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
              }
            }
            SPVM_STRING_BUFFER_add(string_buffer, "arg");
            SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
            if (arg_index != args_length - 1) {
              SPVM_STRING_BUFFER_add(string_buffer, ", ");
            }
          }
        }
        SPVM_STRING_BUFFER_add(string_buffer, ") = ");
        SPVM_STRING_BUFFER_add_address(string_buffer, constant_pool_sub->native_address);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        // Call function
        switch (return_type_code) {
          case SPVM_TYPE_C_CODE_VOID:
            break;
          case SPVM_TYPE_C_CODE_BYTE:
            SPVM_STRING_BUFFER_add(string_buffer, "  int8_t ");
            break;
          case SPVM_TYPE_C_CODE_SHORT:
            SPVM_STRING_BUFFER_add(string_buffer, "  int16_t ");
            break;
          case SPVM_TYPE_C_CODE_INT:
            SPVM_STRING_BUFFER_add(string_buffer, "  int32_t ");
            break;
          case SPVM_TYPE_C_CODE_LONG:
            SPVM_STRING_BUFFER_add(string_buffer, "  int64_t ");
            break;
          case SPVM_TYPE_C_CODE_FLOAT:
            SPVM_STRING_BUFFER_add(string_buffer, "  float ");
            break;
          case SPVM_TYPE_C_CODE_DOUBLE:
            SPVM_STRING_BUFFER_add(string_buffer, "  double ");
            break;
          default:
            SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* ");
        }
        if (!constant_pool_sub->is_void) {
          SPVM_STRING_BUFFER_add(string_buffer, "return_value_native;\n");
        }
        if (args_length > 0) {
          SPVM_STRING_BUFFER_add(string_buffer, "  (*native_address)(api,");
        }
        else {
          SPVM_STRING_BUFFER_add(string_buffer, "  (*native_address)(api");
        }
        {
          int32_t arg_index;
          for (arg_index = 0; arg_index < args_length; arg_index++) {
            int32_t arg_type_id = constant_pool[arg_type_ids_base + arg_index];

            // Argument type code
            SPVM_CONSTANT_POOL_TYPE* constant_pool_arg_type = (SPVM_CONSTANT_POOL_TYPE*)&constant_pool[arg_type_id];
            int32_t arg_type_code = constant_pool_arg_type->code;
            
            switch (arg_type_code) {
              case SPVM_TYPE_C_CODE_BYTE : {
                SPVM_STRING_BUFFER_add(string_buffer, "int8_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_SHORT : {
                SPVM_STRING_BUFFER_add(string_buffer, "int16_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_INT : {
                SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_LONG : {
                SPVM_STRING_BUFFER_add(string_buffer, "int64_t ");
                break;
              }
              case  SPVM_TYPE_C_CODE_FLOAT : {
                SPVM_STRING_BUFFER_add(string_buffer, "float ");
                break;
              }
              case  SPVM_TYPE_C_CODE_DOUBLE : {
                SPVM_STRING_BUFFER_add(string_buffer, "double ");
                break;
              }
              default : {
                SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* ");
              }
            }
            SPVM_STRING_BUFFER_add(string_buffer, "arg");
            SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
            if (arg_index != args_length - 1) {
              SPVM_STRING_BUFFER_add(string_buffer, ", ");
            }
          }
        }
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        if (constant_pool_sub->is_void) {
          SPVM_STRING_BUFFER_add(string_buffer, "  return;\n");
        }
        else {
          SPVM_STRING_BUFFER_add(string_buffer, "  return return_value_native;\n");
        }
        SPVM_STRING_BUFFER_add(string_buffer, "\n");
      }
      // Normal subroutine
      else {
        // Condition flag
        SPVM_STRING_BUFFER_add(string_buffer, "  register int32_t condition_flag;\n");

        SPVM_OPCODE* opcodes = runtime->opcodes;
        int32_t opcode_index = constant_pool_sub->opcode_base;
        
        int32_t loop_break = 0;
        while (1) {
          SPVM_OPCODE* opcode = &(opcodes[opcode_index]);
          switch (opcode->code) {
            case SPVM_OPCODE_C_CODE_NOP:
              abort();
            case SPVM_OPCODE_C_CODE_BOOL_BYTE:
            case SPVM_OPCODE_C_CODE_BOOL_SHORT:
            case SPVM_OPCODE_C_CODE_BOOL_INT:
            case SPVM_OPCODE_C_CODE_BOOL_LONG:
            case SPVM_OPCODE_C_CODE_BOOL_FLOAT:
            case SPVM_OPCODE_C_CODE_BOOL_DOUBLE:
            case SPVM_OPCODE_C_CODE_BOOL_OBJECT:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = !!var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_IS_UNDEF:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " == NULL;\n");
              break;
            case SPVM_OPCODE_C_CODE_IS_NOT_UNDEF:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " != NULL;\n");
              break;
            case SPVM_OPCODE_C_CODE_EQ_BYTE:
            case SPVM_OPCODE_C_CODE_EQ_SHORT:
            case SPVM_OPCODE_C_CODE_EQ_INT:
            case SPVM_OPCODE_C_CODE_EQ_LONG:
            case SPVM_OPCODE_C_CODE_EQ_FLOAT:
            case SPVM_OPCODE_C_CODE_EQ_DOUBLE:
            case SPVM_OPCODE_C_CODE_EQ_OBJECT:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " == var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_NE_BYTE:
            case SPVM_OPCODE_C_CODE_NE_SHORT:
            case SPVM_OPCODE_C_CODE_NE_INT:
            case SPVM_OPCODE_C_CODE_NE_LONG:
            case SPVM_OPCODE_C_CODE_NE_FLOAT:
            case SPVM_OPCODE_C_CODE_NE_DOUBLE:
            case SPVM_OPCODE_C_CODE_NE_OBJECT:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " != var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_GT_BYTE:
            case SPVM_OPCODE_C_CODE_GT_SHORT:
            case SPVM_OPCODE_C_CODE_GT_INT:
            case SPVM_OPCODE_C_CODE_GT_LONG:
            case SPVM_OPCODE_C_CODE_GT_FLOAT:
            case SPVM_OPCODE_C_CODE_GT_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " > var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_GE_BYTE:
            case SPVM_OPCODE_C_CODE_GE_SHORT:
            case SPVM_OPCODE_C_CODE_GE_INT:
            case SPVM_OPCODE_C_CODE_GE_LONG:
            case SPVM_OPCODE_C_CODE_GE_FLOAT:
            case SPVM_OPCODE_C_CODE_GE_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " <= var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_LT_BYTE:
            case SPVM_OPCODE_C_CODE_LT_SHORT:
            case SPVM_OPCODE_C_CODE_LT_INT:
            case SPVM_OPCODE_C_CODE_LT_LONG:
            case SPVM_OPCODE_C_CODE_LT_FLOAT:
            case SPVM_OPCODE_C_CODE_LT_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " < var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_LE_BYTE:
            case SPVM_OPCODE_C_CODE_LE_SHORT:
            case SPVM_OPCODE_C_CODE_LE_INT:
            case SPVM_OPCODE_C_CODE_LE_LONG:
            case SPVM_OPCODE_C_CODE_LE_FLOAT:
            case SPVM_OPCODE_C_CODE_LE_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " <= var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_ADD_BYTE:
            case SPVM_OPCODE_C_CODE_ADD_SHORT:
            case SPVM_OPCODE_C_CODE_ADD_INT:
            case SPVM_OPCODE_C_CODE_ADD_LONG:
            case SPVM_OPCODE_C_CODE_ADD_FLOAT:
            case SPVM_OPCODE_C_CODE_ADD_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " + var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_SUBTRACT_BYTE:
            case SPVM_OPCODE_C_CODE_SUBTRACT_SHORT:
            case SPVM_OPCODE_C_CODE_SUBTRACT_INT:
            case SPVM_OPCODE_C_CODE_SUBTRACT_LONG:
            case SPVM_OPCODE_C_CODE_SUBTRACT_FLOAT:
            case SPVM_OPCODE_C_CODE_SUBTRACT_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " - var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_MULTIPLY_BYTE:
            case SPVM_OPCODE_C_CODE_MULTIPLY_SHORT:
            case SPVM_OPCODE_C_CODE_MULTIPLY_INT:
            case SPVM_OPCODE_C_CODE_MULTIPLY_LONG:
            case SPVM_OPCODE_C_CODE_MULTIPLY_FLOAT:
            case SPVM_OPCODE_C_CODE_MULTIPLY_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " * var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_DIVIDE_BYTE:
            case SPVM_OPCODE_C_CODE_DIVIDE_SHORT:
            case SPVM_OPCODE_C_CODE_DIVIDE_INT:
            case SPVM_OPCODE_C_CODE_DIVIDE_LONG: {
              SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, " == 0, 0)) { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* exception = api->new_string(api, \"0 division\", 0); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    api->set_exception(api, exception); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    goto label_SPVM_OPCODE_C_CODE_CROAK; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "  } else { \\\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    \n");
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " / var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
              break;
            }
            case SPVM_OPCODE_C_CODE_DIVIDE_FLOAT:
            case SPVM_OPCODE_C_CODE_DIVIDE_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " / var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_REMAINDER_BYTE:
            case SPVM_OPCODE_C_CODE_REMAINDER_SHORT:
            case SPVM_OPCODE_C_CODE_REMAINDER_INT:
            case SPVM_OPCODE_C_CODE_REMAINDER_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " % var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_REMAINDER_FLOAT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (float)fmod((double)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ", (double)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ");\n");
              break;
            case SPVM_OPCODE_C_CODE_REMAINDER_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = fmod(var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ", var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ");\n");
              break;
            case SPVM_OPCODE_C_CODE_LEFT_SHIFT_BYTE:
            case SPVM_OPCODE_C_CODE_LEFT_SHIFT_SHORT:
            case SPVM_OPCODE_C_CODE_LEFT_SHIFT_INT:
            case SPVM_OPCODE_C_CODE_LEFT_SHIFT_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " << var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_BYTE:
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_SHORT:
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_INT:
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " >> var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_UNSIGNED_BYTE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int8_t)((uint8_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " >> var)");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_UNSIGNED_SHORT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int16_t)((uint16_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " >> var)");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_UNSIGNED_INT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int32_t)((uint32_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " >> var)");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_RIGHT_SHIFT_UNSIGNED_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int64_t)((uint64_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " >> var)");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_BIT_AND_BYTE:
            case SPVM_OPCODE_C_CODE_BIT_AND_SHORT:
            case SPVM_OPCODE_C_CODE_BIT_AND_INT:
            case SPVM_OPCODE_C_CODE_BIT_AND_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " & var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_BIT_OR_BYTE:
            case SPVM_OPCODE_C_CODE_BIT_OR_SHORT:
            case SPVM_OPCODE_C_CODE_BIT_OR_INT:
            case SPVM_OPCODE_C_CODE_BIT_OR_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " | var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_BIT_XOR_BYTE:
            case SPVM_OPCODE_C_CODE_BIT_XOR_SHORT:
            case SPVM_OPCODE_C_CODE_BIT_XOR_INT:
            case SPVM_OPCODE_C_CODE_BIT_XOR_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " ^ var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_NEGATE_BYTE:
            case SPVM_OPCODE_C_CODE_NEGATE_SHORT:
            case SPVM_OPCODE_C_CODE_NEGATE_INT:
            case SPVM_OPCODE_C_CODE_NEGATE_LONG:
            case SPVM_OPCODE_C_CODE_NEGATE_FLOAT:
            case SPVM_OPCODE_C_CODE_NEGATE_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = -var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_COMPLEMENT_BYTE:
            case SPVM_OPCODE_C_CODE_COMPLEMENT_SHORT:
            case SPVM_OPCODE_C_CODE_COMPLEMENT_INT:
            case SPVM_OPCODE_C_CODE_COMPLEMENT_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = ~var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_INC_BYTE:
            case SPVM_OPCODE_C_CODE_INC_SHORT:
            case SPVM_OPCODE_C_CODE_INC_INT:
            case SPVM_OPCODE_C_CODE_INC_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " += ");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_BYTE:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_BYTE:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_BYTE:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_BYTE:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_BYTE:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_BYTE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int8_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_SHORT:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_SHORT:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_SHORT:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_SHORT:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_SHORT:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_SHORT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int16_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_INT:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_INT:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_INT:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_INT:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_INT:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_INT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int32_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_LONG:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_LONG:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_LONG:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_LONG:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_LONG:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_LONG:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (int64_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_FLOAT:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_FLOAT:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_FLOAT:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_FLOAT:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_FLOAT:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_FLOAT:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (float)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
            case SPVM_OPCODE_C_CODE_CONVERT_BYTE_TO_DOUBLE:
            case SPVM_OPCODE_C_CODE_CONVERT_SHORT_TO_DOUBLE:
            case SPVM_OPCODE_C_CODE_CONVERT_INT_TO_DOUBLE:
            case SPVM_OPCODE_C_CODE_CONVERT_LONG_TO_DOUBLE:
            case SPVM_OPCODE_C_CODE_CONVERT_DOUBLE_TO_DOUBLE:
            case SPVM_OPCODE_C_CODE_CONVERT_FLOAT_TO_DOUBLE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = (double)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
            case SPVM_OPCODE_C_CODE_LOAD_CONSTANT_0:
            case SPVM_OPCODE_C_CODE_LOAD_CONSTANT2_0:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = 0;\n");
              break;
            case SPVM_OPCODE_C_CODE_UNDEF:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = NULL;\n");
              break;
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_BYTE:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_SHORT:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_INT:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_LONG:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_FLOAT:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_DOUBLE:
            case SPVM_OPCODE_C_CODE_ARRAY_LOAD_OBJECT:
            {
              SPVM_STRING_BUFFER_add(string_buffer, "  // Load Array Element\n");
              
              char* element_type = NULL;
              switch (opcode->code) {
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_BYTE:
                  element_type = "int8_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_SHORT:
                  element_type = "int16_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_INT:
                  element_type = "int32_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_LONG:
                  element_type = "int64_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_FLOAT:
                  element_type = "float";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_DOUBLE:
                  element_type = "double";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_LOAD_OBJECT:
                  element_type = "SPVM_API_OBJECT*";
              }
              
              SPVM_STRING_BUFFER_add(string_buffer, "  { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* array = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, "; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    int32_t index = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(array == NULL, 0)) { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_API_OBJECT* exception = api->new_string(api, \"Array must not be undef\", 0); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      api->set_exception(api, exception); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      goto label_SPVM_OPCODE_C_CODE_CROAK; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      if (__builtin_expect(index < 0 || index >= *(int32_t*)((intptr_t)array + SPVM_INFO_OBJECT_LENGTH_BYTE_OFFSET), 0)) { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        SPVM_API_OBJECT* exception = api->new_string(api, \"Index is out of range\", 0); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        api->set_exception(api, exception); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        goto label_SPVM_OPCODE_C_CODE_CROAK; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      else { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = *(");
              SPVM_STRING_BUFFER_add(string_buffer, element_type);
              SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)array + SPVM_INFO_OBJECT_HEADER_BYTE_SIZE + sizeof(");
              SPVM_STRING_BUFFER_add(string_buffer, element_type);
              SPVM_STRING_BUFFER_add(string_buffer, ") * index); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
              break;
            }
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_BYTE:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_SHORT:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_INT:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_LONG:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_FLOAT:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_DOUBLE:
            case SPVM_OPCODE_C_CODE_ARRAY_STORE_OBJECT:
            {
              SPVM_STRING_BUFFER_add(string_buffer, "  // Store Array Element\n");

              char* element_type = NULL;
              switch (opcode->code) {
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_BYTE:
                  element_type = "int8_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_SHORT:
                  element_type = "int16_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_INT:
                  element_type = "int32_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_LONG:
                  element_type = "int64_t";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_FLOAT:
                  element_type = "float";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_DOUBLE:
                  element_type = "double";
                  break;
                case SPVM_OPCODE_C_CODE_ARRAY_STORE_OBJECT:
                  element_type = "SPVM_API_OBJECT*";
              }
              
              SPVM_STRING_BUFFER_add(string_buffer, "  { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* array = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    int32_t index = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(array == NULL, 0)) { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_API_OBJECT* exception = api->new_string(api, \"Array must not be undef\", 0); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      api->set_exception(api, exception); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      goto label_SPVM_OPCODE_C_CODE_CROAK; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      if (__builtin_expect(index < 0 || index >= *(int32_t*)((intptr_t)array + SPVM_INFO_OBJECT_LENGTH_BYTE_OFFSET), 0)) { \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        SPVM_API_OBJECT* exception = api->new_string(api, \"Index is out of range\", 0); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        api->set_exception(api, exception); \n");
              SPVM_STRING_BUFFER_add(string_buffer, "        goto label_SPVM_OPCODE_C_CODE_CROAK; \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "      else { \n");
              
              if (opcode->code == SPVM_OPCODE_C_CODE_ARRAY_STORE_OBJECT) {
                SPVM_STRING_BUFFER_add(string_buffer, "        SPVM_API_OBJECT** object_address = (SPVM_API_OBJECT**)((intptr_t)array + SPVM_INFO_OBJECT_HEADER_BYTE_SIZE + sizeof(void*) * index\n");
                SPVM_STRING_BUFFER_add(string_buffer, "        if (*object_address != NULL) {\n");
                SPVM_STRING_BUFFER_add(string_buffer, "          if (SPVM_INLINE_GET_REF_COUNT(*object_address) > 1) {\n");
                SPVM_STRING_BUFFER_add(string_buffer, "            SPVM_INLINE_DEC_REF_COUNT_ONLY(*object_address);\n");
                SPVM_STRING_BUFFER_add(string_buffer, "          }\n");
                SPVM_STRING_BUFFER_add(string_buffer, "          else {\n");
                SPVM_STRING_BUFFER_add(string_buffer, "            api->dec_ref_count(api, *object_address);\n");
                SPVM_STRING_BUFFER_add(string_buffer, "          }\n");
                SPVM_STRING_BUFFER_add(string_buffer, "        }\n");
              }

              SPVM_STRING_BUFFER_add(string_buffer, "        *(");
              SPVM_STRING_BUFFER_add(string_buffer, element_type);
              SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)array + SPVM_INFO_OBJECT_HEADER_BYTE_SIZE + sizeof(");
              SPVM_STRING_BUFFER_add(string_buffer, element_type);
              SPVM_STRING_BUFFER_add(string_buffer, ") * index) = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");

              if (opcode->code == SPVM_OPCODE_C_CODE_ARRAY_STORE_OBJECT) {
                SPVM_STRING_BUFFER_add(string_buffer, "        if (*object_address != NULL) {\n");
                SPVM_STRING_BUFFER_add(string_buffer, "          SPVM_INLINE_INC_REF_COUNT(*object_address);\n");
                SPVM_STRING_BUFFER_add(string_buffer, "        }\n");
              }

              SPVM_STRING_BUFFER_add(string_buffer, "      } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
              SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
              
              break;
            }
            case SPVM_OPCODE_C_CODE_MOVE:
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              break;
            case SPVM_OPCODE_C_CODE_INC_REF_COUNT: {
              SPVM_STRING_BUFFER_add(string_buffer, "  // Increment reference count\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  if (var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " != NULL) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_INLINE_INC_REF_COUNT(var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ");\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              
              break;
            }
            case SPVM_OPCODE_C_CODE_DEC_REF_COUNT: {
              SPVM_STRING_BUFFER_add(string_buffer, "  // Decrement reference count\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  if (var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " != NULL) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    if (SPVM_INLINE_GET_REF_COUNT(var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ") > 1) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_INLINE_DEC_REF_COUNT_ONLY(var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ")\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    else {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "      api->dec_ref_count(api, var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, ")\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_NEW_OBJECT: {
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t type_id = ");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_object(api, type_id);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, "  = object;\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_NEW_BYTE_ARRAY:
            case SPVM_OPCODE_C_CODE_NEW_SHORT_ARRAY:
            case SPVM_OPCODE_C_CODE_NEW_INT_ARRAY:
            case SPVM_OPCODE_C_CODE_NEW_LONG_ARRAY:
            case SPVM_OPCODE_C_CODE_NEW_FLOAT_ARRAY:
            case SPVM_OPCODE_C_CODE_NEW_DOUBLE_ARRAY:
            {
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t length = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              switch (opcode->code) {
                case SPVM_OPCODE_C_CODE_NEW_BYTE_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_byte_array(api, length);\n");
                  break;
                case SPVM_OPCODE_C_CODE_NEW_SHORT_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_short_array(api, length);\n");
                  break;
                case SPVM_OPCODE_C_CODE_NEW_INT_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_int_array(api, length);\n");
                  break;
                case SPVM_OPCODE_C_CODE_NEW_LONG_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_long_array(api, length);\n");
                  break;
                case SPVM_OPCODE_C_CODE_NEW_FLOAT_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_float_array(api, length);\n");
                  break;
                case SPVM_OPCODE_C_CODE_NEW_DOUBLE_ARRAY:
                  SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = api->new_double_array(api, length);\n");
                  break;
              }
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = object;\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_NEW_OBJECT_ARRAY: {
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t element_type_id = ");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, "  ;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t length = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, "  ;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = ((SPVM_API*)api)->new_object_array(api, element_type_id, length);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, "   = object;\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_NEW_STRING: {
              SPVM_STRING_BUFFER_add(string_buffer, "  int32_t name_id = ");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "SPVM_API_OBJECT* string = ((SPVM_API*)api)->new_string(api, (char*)&SPVM_INFO_CONSTANT_POOL[name_id + 1], SPVM_INFO_CONSTANT_POOL[name_id]);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = string;\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_ARRAY_LENGTH: {
              SPVM_STRING_BUFFER_add(string_buffer, "  // Array length");
              SPVM_STRING_BUFFER_add(string_buffer, "  if (var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " == NULL) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* exception = ((SPVM_API*)api)->new_string(api, \"Can't get array length of undef value.\", 0);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    ((SPVM_API*)api)->set_exception(api, exception);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    goto label_SPVM_OPCODE_C_CODE_CROAK;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = *(int32_t*)((intptr_t)var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, " + SPVM_INFO_OBJECT_LENGTH_BYTE_OFFSET);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              break;
            }
            case SPVM_OPCODE_C_CODE_GET_FIELD_BYTE:
            case SPVM_OPCODE_C_CODE_GET_FIELD_SHORT:
            case SPVM_OPCODE_C_CODE_GET_FIELD_INT:
            case SPVM_OPCODE_C_CODE_GET_FIELD_LONG:
            case SPVM_OPCODE_C_CODE_GET_FIELD_FLOAT:
            case SPVM_OPCODE_C_CODE_GET_FIELD_DOUBLE:
            case SPVM_OPCODE_C_CODE_GET_FIELD_OBJECT:
            {
              char* field_type = NULL;
              switch (opcode->code) {
                case SPVM_OPCODE_C_CODE_GET_FIELD_BYTE:
                  field_type = "int8_t";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_SHORT:
                  field_type = "int16_t";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_INT:
                  field_type = "int32_t";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_LONG:
                  field_type = "int64_t";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_FLOAT:
                  field_type = "float";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_DOUBLE:
                  field_type = "double";
                  break;
                case SPVM_OPCODE_C_CODE_GET_FIELD_OBJECT:
                  field_type = "SPVM_API_OBJECT*";
              }
              
              int32_t field_id = opcode->operand2;
              SPVM_CONSTANT_POOL_FIELD* constant_pool_field = (SPVM_CONSTANT_POOL_FIELD*)&constant_pool[field_id];
              int32_t field_byte_offset = constant_pool_field->byte_offset;
              
              SPVM_STRING_BUFFER_add(string_buffer, "  // Get field\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
              SPVM_STRING_BUFFER_add(string_buffer, "  ;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(object == NULL, 0)) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* exception = api->new_string(api, \"Object must be not undef.\", 0);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    api->set_exception(api, exception);");
              SPVM_STRING_BUFFER_add(string_buffer, "    goto label_SPVM_OPCODE_C_CODE_CROAK;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, " = *(");
              SPVM_STRING_BUFFER_add(string_buffer, field_type);
              SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)object + SPVM_INFO_OBJECT_HEADER_BYTE_SIZE + ");
              SPVM_STRING_BUFFER_add_int(string_buffer, field_byte_offset);
              SPVM_STRING_BUFFER_add(string_buffer, ");\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              
              break;
            }
            case SPVM_OPCODE_C_CODE_SET_FIELD_BYTE:
            case SPVM_OPCODE_C_CODE_SET_FIELD_SHORT:
            case SPVM_OPCODE_C_CODE_SET_FIELD_INT:
            case SPVM_OPCODE_C_CODE_SET_FIELD_LONG:
            case SPVM_OPCODE_C_CODE_SET_FIELD_FLOAT:
            case SPVM_OPCODE_C_CODE_SET_FIELD_DOUBLE:
            {
              char* field_type = NULL;
              switch (opcode->code) {
                case SPVM_OPCODE_C_CODE_SET_FIELD_BYTE:
                  field_type = "int8_t";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_SHORT:
                  field_type = "int16_t";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_INT:
                  field_type = "int32_t";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_LONG:
                  field_type = "int64_t";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_FLOAT:
                  field_type = "float";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_DOUBLE:
                  field_type = "double";
                  break;
                case SPVM_OPCODE_C_CODE_SET_FIELD_OBJECT:
                  field_type = "SPVM_API_OBJECT*";
              }
              
              int32_t field_id = opcode->operand1;
              SPVM_CONSTANT_POOL_FIELD* constant_pool_field = (SPVM_CONSTANT_POOL_FIELD*)&constant_pool[field_id];
              int32_t field_byte_offset = constant_pool_field->byte_offset;
              
              SPVM_STRING_BUFFER_add(string_buffer, "  // Set field\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_API_OBJECT* object = var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
              SPVM_STRING_BUFFER_add(string_buffer, "  ;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(object == NULL, 0)) {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_API_OBJECT* exception = api->new_string(api, \"Object must be not undef.\", 0);\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    api->set_exception(api, exception);");
              SPVM_STRING_BUFFER_add(string_buffer, "    goto label_SPVM_OPCODE_C_CODE_CROAK;\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
              SPVM_STRING_BUFFER_add(string_buffer, "    *(");
              SPVM_STRING_BUFFER_add(string_buffer, field_type);
              SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)object + SPVM_INFO_OBJECT_HEADER_BYTE_SIZE + ");
              SPVM_STRING_BUFFER_add_int(string_buffer, field_byte_offset);
              SPVM_STRING_BUFFER_add(string_buffer, ") = ");
              SPVM_STRING_BUFFER_add(string_buffer, "var");
              SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
              SPVM_STRING_BUFFER_add(string_buffer, ";\n");
              SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
              
              break;
            }
            
            case SPVM_OPCODE_C_CODE_RETURN_BYTE:
            case SPVM_OPCODE_C_CODE_RETURN_SHORT:
            case SPVM_OPCODE_C_CODE_RETURN_INT:
            case SPVM_OPCODE_C_CODE_RETURN_LONG:
            case SPVM_OPCODE_C_CODE_RETURN_FLOAT:
            case SPVM_OPCODE_C_CODE_RETURN_DOUBLE:
            case SPVM_OPCODE_C_CODE_RETURN_OBJECT:
            case SPVM_OPCODE_C_CODE_RETURN_VOID:
            case SPVM_OPCODE_C_CODE_CROAK:
              loop_break = 1;
              break;
          }
          if (loop_break) {
            break;
          }
          opcode_index++;
        }
      }

      // Close subroutine
      SPVM_STRING_BUFFER_add(string_buffer, "}\n");
      SPVM_STRING_BUFFER_add(string_buffer, "\n");
    }
  }
  
  // warn("%s", string_buffer->buffer);
}
