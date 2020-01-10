  SOLE TESTS

    Create exe file by spvmcc command
      
      yacc/bison.sh && perl Makefile.PL && make && perl -Mblib blib/script/spvmcc -B solo/spvm_build -I solo/lib -o solo/myexe TestCase && solo/myexe foo bar
      
    SPVM solo test command

       # Normal run
       yacc/bison.sh && perl Makefile.PL && make && perl solo/solo_Makefile.PL && make -f solo/Makefile && ./solo/main foo bar

       # Debug run - Print AST, package information, operaion codes
       yacc/bison.sh && perl Makefile.PL --OPTIMIZE="-O0 -g" --DEFINE=SPVM_DEBUG_DUMP --DEFINE=SPVM_DEBUG_OBJECT_COUNT && make && perl solo/solo_Makefile.PL  --OPTIMIZE="-O0 -g" --DEFINE=SPVM_DEBUG_DUMP --DEFINE=SPVM_DEBUG_OBJECT_COUNT && make -f solo/Makefile && ./solo/main foo bar

       # Debug run - Only syntax check, don't use SPVM::CORE and don't compile core modules
       yacc/bison.sh && perl Makefile.PL --OPTIMIZE="-O0 -g" --DEFINE=SPVM_DONT_USE_SPVM_CORE --DEFINE=SPVM_DONT_COMPILE_CORE_MODULES && make && perl solo/solo_Makefile.PL --OPTIMIZE="-O0 -g" && make -f solo/Makefile && ./solo/main foo bar

       # Debug run - Print yacc result, don't use SPVM::CORE and don't compile core modules
       yacc/bison.sh && perl Makefile.PL --OPTIMIZE="-O0 -g" --DEFINE=SPVM_DEBUG_YACC --DEFINE=SPVM_DONT_USE_SPVM_CORE --DEFINE=SPVM_DONT_COMPILE_CORE_MODULES && make && perl solo/solo_Makefile.PL  --OPTIMIZE="-O0 -g" --DEFINE=SPVM_DEBUG_YACC && make -f solo/Makefile && ./solo/main foo bar
    
       # Cleanup
       make -f solo/Makefile clean
    
    See batcktrace of core dump

      gdb solo/main core.*

    What is solo test command?

      solo test is used for debgug.
      
      SPVM is compiled to dynamic link library by -O3 option.
      
      this is difficult to debug because debug information is removed and it is not simple machine code.

    See core dump

      If you use Debug run, you can see core dump and back trace by gdb command

        gdb spvm core.*
        
        bt
