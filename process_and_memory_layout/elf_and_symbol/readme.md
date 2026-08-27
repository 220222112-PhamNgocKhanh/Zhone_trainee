In this part, I will create a shared library and simulate the bug situation: Missing symbol in dynamic linking.

First, use the `foo.c` to build the shared lib by this command: `gcc -fPIC -shared foo.c -o libfoo.so`

Now we have the shared lib, so we can link this library and the main source. However, we need to use some flags to link. Run this command to link: `gcc main.c -L. -lfoo     -Wl,--allow-shlib-undefined     -Wl,-rpath,'$ORIGIN'     -o main`

Now when we have the execute file, we can check the symbol table of this file and the shared lib file.
Run: `ldd main`: We can see all shared libraries that the program need: 
```
linux-vdso.so.1 (0x00007ffecb4fc000)
libfoo.so => /home/khanhnp/lab/Zhone_trainee/process_and_memory_layout/elf_and_symbol/./libfoo.so (0x00007ca1436d4000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ca143400000)
/lib64/ld-linux-x86-64.so.2 (0x00007ca1436e0000)
```
Run: `nm main` and `nm libfoo.so`: 

The result of main: 
```
000000000000038c r __abi_tag
0000000000004010 B __bss_start
0000000000004010 b completed.0
                 w __cxa_finalize@GLIBC_2.2.5
0000000000004000 D __data_start
0000000000004000 W data_start
00000000000010d0 t deregister_tm_clones
0000000000001140 t __do_global_dtors_aux
0000000000003d90 d __do_global_dtors_aux_fini_array_entry
0000000000004008 D __dso_handle
0000000000003d98 d _DYNAMIC
0000000000004010 D _edata
0000000000004018 B _end
00000000000011b8 T _fini
                 U foo
0000000000001180 t frame_dummy
0000000000003d88 d __frame_dummy_init_array_entry
0000000000002100 r __FRAME_END__
                 U getchar@GLIBC_2.2.5
0000000000003fa8 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000002020 r __GNU_EH_FRAME_HDR
0000000000001000 T _init
0000000000002000 R _IO_stdin_used
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U __libc_start_main@GLIBC_2.34
0000000000001189 T main
                 U printf@GLIBC_2.2.5
0000000000001100 t register_tm_clones
00000000000010a0 T _start
0000000000004010 D __TMC_END_
```
We can see, the foo symbol is undefined, this is normal with shared library. It look like other functions like `getchar` or `printf`

Now check to the result of `libfoo.so`

```
0000000000004030 b completed.0
                 w __cxa_finalize@GLIBC_2.2.5
0000000000001080 t deregister_tm_clones
00000000000010f0 t __do_global_dtors_aux
0000000000003e18 d __do_global_dtors_aux_fini_array_entry
0000000000004028 d __dso_handle
0000000000003e20 d _DYNAMIC
0000000000001158 t _fini
0000000000001139 T foo
0000000000001130 t frame_dummy
0000000000003e10 d __frame_dummy_init_array_entry
00000000000020d0 r __FRAME_END__
0000000000004000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000002014 r __GNU_EH_FRAME_HDR
0000000000001000 t _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U missing_function
                 U puts@GLIBC_2.2.5
00000000000010b0 t register_tm_clones
0000000000004030 d __TMC_END__
```
The `foo` symbol have in the table. Nevertherless, we the `missing function` symbol don't have. If we run the program, we can see it still running because the first part don't use the `foo` function. If we press the `enter`, the foo function will be called, the result look like: 
```
the program normally run
foo() is running
./main: symbol lookup error: /home/khanhnp/lab/Zhone_trainee/process_and_memory_layout/elf_and_symbol/libfoo.so: undefined symbol: missing_function
```

