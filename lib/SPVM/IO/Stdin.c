#include "spvm_native.h"
#include <stdio.h>

// import _setmode and _O_BINARY
#ifdef _WIN32
#include <io.h>
#endif

static const char* MFILE = "SPVM/IO/Stdin.c";

int32_t SPNATIVE__SPVM__IO__Stdin__readline(SPVM_ENV* env, SPVM_VALUE* stack) {

  int32_t scope_id = env->enter_scope(env);
  
  int32_t capacity = 80;
  void* obj_buffer = env->new_barray(env, capacity);
  int8_t* buffer = env->get_elems_byte(env, obj_buffer);
  
  int32_t pos = 0;
  int32_t end_is_eof = 0;
  while (1) {
    int32_t ch = getchar();
    if (ch == EOF) {
      end_is_eof = 1;
      break;
    }
    else {
      if (pos >= capacity) {
        // Extend buffer capacity
        int32_t new_capacity = capacity * 2;
        void* new_object_buffer = env->new_barray(env, new_capacity);
        int8_t* new_buffer = env->get_elems_byte(env, new_object_buffer);
        memcpy(new_buffer, buffer, capacity);
        
        int32_t removed = env->remove_mortal(env, scope_id, obj_buffer);
        assert(removed);
        
        capacity = new_capacity;
        obj_buffer = new_object_buffer;
        buffer = new_buffer;
      }
      
      if (ch == '\n') {
        buffer[pos] = ch;
        pos++;
        break;
      }
      else {
        buffer[pos] = ch;
        pos++;
      }
    }
  }
  
  if (pos > 0 || !end_is_eof) {
    void* oline;
    if (pos == 0) {
      oline = env->new_barray(env, 0);
    }
    else {
      oline = env->new_barray(env, pos);
      int8_t* line = env->get_elems_byte(env, oline);
      memcpy(line, buffer, pos);
    }
    
    stack[0].oval = oline;
  }
  else {
    stack[0].oval = NULL;
  }
  
  return SPVM_SUCCESS;
}

int32_t SPNATIVE__SPVM__IO__Stdin__set_binmode(SPVM_ENV* env, SPVM_VALUE* stack) {

#ifdef _WIN32
  int32_t binmode = stack[0].ival;
  if (binmode) {
    _setmode(_fileno(stdin), _O_BINARY);
  }
  else {
    _setmode(_fileno(stdin), _O_TEXT);
  }
#endif
  
  return SPVM_SUCCESS;
}