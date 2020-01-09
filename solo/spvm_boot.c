#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "spvm_native.h"
#include "spvm_bootstrap.h"

#include "spvm_runtime_api.h"

static int32_t SPVM_BOOT_call_entry_point_sub(SPVM_ENV* env, const char* package_name, int32_t argc, const char *argv[]) {
  
  // Package
  int32_t sub_id = env->get_sub_id(env, package_name, "main", "int(string[])");
  
  if (sub_id < 0) {
    fprintf(stderr, "Can't find entry point package %s\n", package_name);
    exit(EXIT_FAILURE);
  }
  
  // Enter scope
  int32_t scope_id = env->enter_scope(env);
  
  // new byte[][args_length] object
  int32_t arg_type_basic_id = env->get_basic_type_id(env, "byte");
  void* cmd_args_obj = env->new_muldim_array(env, arg_type_basic_id, 1, argc);
  
  // Set command line arguments
  for (int32_t arg_index = 0; arg_index < argc; arg_index++) {
    void* cmd_arg_obj = env->new_string_len(env, argv[arg_index], strlen(argv[arg_index]));
    env->set_elem_object(env, cmd_args_obj, arg_index, cmd_arg_obj);
  }
  
  SPVM_VALUE stack[255];
  stack[0].oval = cmd_args_obj;
  
  // Run
  int32_t exception_flag = env->call_sub(env, sub_id, stack);
  
  int32_t status_code;
  if (exception_flag) {
    // Exception
    void* string = env->exception_object;
    int8_t* bytes = env->get_elems_byte(env, string);
    int32_t string_length = env->length(env, string);
    {
      int32_t i;
      for (i = 0; i < string_length; i++) {
        fputc((char)bytes[i], stderr);
      }
    }
    printf("\n");
    status_code = 255;
  }
  else {
    status_code = stack[0].ival;
  }
  
  // Leave scope
  env->leave_scope(env, scope_id);
  
  return status_code;
}

int32_t main(int32_t argc, const char *argv[]) {
  
  SPVM_ENV* env = SPVM_BOOTSTRAP_create_env();
  
  // Package name
  const char* package_name = "Main";
  
  // env->set_compiler_debug(env, 1);
  
  // Get script directory
  const char* cur_script_name = argv[0];
  int32_t cur_script_name_length = (int32_t)strlen(argv[0]);
  char* cur_script_dir = malloc(cur_script_name_length + 1);
  memcpy(cur_script_dir, cur_script_name, cur_script_name_length);
  cur_script_dir[cur_script_name_length] = '\0';
  int32_t found_sep = 0;
  for (int32_t i = cur_script_name_length - 1; i >= 0; i--) {
    if (cur_script_dir[i] == '/' || cur_script_dir[i] == '\\') {
      cur_script_dir[i] = '\0';
      found_sep = 1;
      break;
    }
  }
  if (!found_sep) {
    cur_script_dir[0] = '.';
    cur_script_dir[1] = '\0';
  }
  
  // Add module path
  env->push_compiler_module_paths(env, cur_script_dir);
  
  // Create env
  SPVM_RUNTIME_API_compile(NULL, package_name, env);

  if (env == NULL) {
    exit(1);
  }
  
  // Call begin blocks
  env->call_begin_blocks(env);

  // Call entry point sub
  int32_t status_code = SPVM_BOOT_call_entry_point_sub(env, package_name, argc, argv);
  
  env->free_env(env);
  
  return status_code;
}

