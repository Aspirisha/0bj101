# Notes on linking process

From https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.symtab.html:

* Each object file contains symbol table with entries (for 64bit platform):

```c++
typedef struct {
	// index into the object file's symbol string table, where symbol name (null-terminated) is stored
	Elf64_Word	st_name;
	// contains symbol binding (higher 4 bits) and symbol type (lower 4 bits)
	unsigned char	st_info;
	// This member currently specifies a symbol's visibility (lower 2 bits)
	unsigned char	st_other;
	// section header table index in relation to which (??) this symbol is defined
	Elf64_Half	st_shndx;
	// This member gives the value of the associated symbol. Depending on the context, this may be an absolute value, an address, and so on; details appear below. 
	Elf64_Addr	st_value;
	// Many symbols have associated sizes. For example, a data object's size is the number of bytes contained in the object; 
	// This member holds 0 if the symbol has no size or an unknown size. 
	Elf64_Xword	st_size; 
} Elf64_Sym;
```



### Symbol binding

Each symbol may be defined with one of following binding types:
* `STB_LOCAL` - then it has local binding and is not accessible outside of object file
* `STB_GLOBAL` - the symbol is visible to all object files being combined, so if there is another object file with undefined reference to a symbol defined in this object file with this binding rules, it will be resolved to this symbol
* `STB_WEAK` - same as global, but allows multiple definitions of the symbol: when combining multiple object files, linker will forbid having more than one `STB_GLOBAL` entry for the same symbol, while it is ok to have multiple weakly bound definitions and 1 globally bound definition.

From https://akkadia.org/drepper/dsohowto.pdf:
Note that a definition in a DSO being *weak* has no effects. Weak definitions only play a role in static linking, and we can have multiple global definitions of the same symbol in distinct DSOs linked dynamically to an executable/other dso. Side note - of course, we will still have problem if we link multiple object files / static libraries when *building* dso - important thing is whether multiple global defintions during linking appear in dynamic libraries or not.


### Inspecting symbol table
Let's call `objdump -tC build/multiple_weak_symbols/libc.so`, which prints symbol table information of the dynamic library

```
objdump -tC build/multiple_weak_symbols/libc.so 

build/multiple_weak_symbols/libc.so:     file format elf64-x86-64

SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000 crtstuff.c
0000000000001040 l     F .text  0000000000000000 deregister_tm_clones
0000000000001070 l     F .text  0000000000000000 register_tm_clones
00000000000010b0 l     F .text  0000000000000000 __do_global_dtors_aux
0000000000004024 l     O .bss   0000000000000001 completed.0
0000000000003e78 l     O .fini_array    0000000000000000 __do_global_dtors_aux_fini_array_entry
00000000000010f0 l     F .text  0000000000000000 frame_dummy
0000000000003e70 l     O .init_array    0000000000000000 __frame_dummy_init_array_entry
0000000000000000 l    df *ABS*  0000000000000000 c.cpp
0000000000000000 l    df *ABS*  0000000000000000 crtstuff.c
0000000000002000 l     O .eh_frame      0000000000000000 __FRAME_END__
0000000000000000 l    df *ABS*  0000000000000000 
0000000000003e80 l     O .dynamic       0000000000000000 _DYNAMIC
0000000000004028 l     O .data  0000000000000000 __TMC_END__
0000000000004018 l     O .data  0000000000000000 __dso_handle
0000000000001000 l     F .init  0000000000000000 _init
00000000000010fc l     F .fini  0000000000000000 _fini
0000000000004000 l     O .got.plt       0000000000000000 _GLOBAL_OFFSET_TABLE_
0000000000000000  w      *UND*  0000000000000000 __cxa_finalize
0000000000004020 g     O .data  0000000000000004 global_var_in_dso
0000000000000000  w      *UND*  0000000000000000 _ITM_registerTMCloneTable
0000000000000000  w      *UND*  0000000000000000 _ITM_deregisterTMCloneTable
0000000000000000  w      *UND*  0000000000000000 __gmon_start__
```

We see it has only single global symbol named `global_var_in_dso` (`g` flag in the first column of flags), all the other symbols are either local (`l`) or neither local nor global (space flag in the first column), like `__cxa_finalize`. The four symbols that are neither local nor global here also appear to be weak (`w` flag in the second column). 

Calling `objdump -tC build/multiple_weak_symbols/multiple_weaks` on executable file:

```
SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000              Scrt1.o
000000000000038c l     O .note.ABI-tag  0000000000000020              __abi_tag
0000000000000000 l    df *ABS*  0000000000000000              crtstuff.c
00000000000010f0 l     F .text  0000000000000000              deregister_tm_clones
0000000000001120 l     F .text  0000000000000000              register_tm_clones
0000000000001160 l     F .text  0000000000000000              __do_global_dtors_aux
0000000000004154 l     O .bss   0000000000000001              completed.0
0000000000003d70 l     O .fini_array    0000000000000000              __do_global_dtors_aux_fini_array_entry
00000000000011a0 l     F .text  0000000000000000              frame_dummy
0000000000003d60 l     O .init_array    0000000000000000              __frame_dummy_init_array_entry
0000000000000000 l    df *ABS*  0000000000000000              a.cpp
0000000000000000 l    df *ABS*  0000000000000000              b.cpp
0000000000000000 l    df *ABS*  0000000000000000              main.cpp
0000000000004155 l     O .bss   0000000000000001              std::__ioinit
000000000000127b l     F .text  0000000000000056              __static_initialization_and_destruction_0(int, int)
00000000000012d1 l     F .text  0000000000000019              _GLOBAL__sub_I_main
0000000000000000 l    df *ABS*  0000000000000000              crtstuff.c
0000000000002178 l     O .eh_frame      0000000000000000              __FRAME_END__
0000000000000000 l    df *ABS*  0000000000000000              
0000000000002048 l       .eh_frame_hdr  0000000000000000              __GNU_EH_FRAME_HDR
0000000000003d78 l     O .dynamic       0000000000000000              _DYNAMIC
0000000000003f98 l     O .got   0000000000000000              _GLOBAL_OFFSET_TABLE_
0000000000004020 g       .data  0000000000000000              _edata
0000000000004000  w      .data  0000000000000000              data_start
0000000000004010  w    O .data  0000000000000004              var
0000000000002000 g     O .rodata        0000000000000004              _IO_stdin_used
0000000000000000  w    F *UND*  0000000000000000              __cxa_finalize@GLIBC_2.2.5
000000000000401c g     O .data  0000000000000004              global_var
00000000000011a9 g     F .text  00000000000000d2              main
0000000000004008 g     O .data  0000000000000000              .hidden __dso_handle
00000000000012ec g     F .fini  0000000000000000              .hidden _fini
0000000000000000       F *UND*  0000000000000000              __libc_start_main@GLIBC_2.34
0000000000000000       F *UND*  0000000000000000              __cxa_atexit@GLIBC_2.2.5
00000000000010c0 g     F .text  0000000000000026              _start
0000000000000000       F *UND*  0000000000000000              std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@GLIBCXX_3.4
0000000000001000 g     F .init  0000000000000000              .hidden _init
0000000000004020 g     O .data  0000000000000000              .hidden __TMC_END__
0000000000004040 g     O .bss   0000000000000110              std::cout@GLIBCXX_3.4
0000000000004000 g       .data  0000000000000000              __data_start
0000000000004158 g       .bss   0000000000000000              _end
0000000000004020 g       .bss   0000000000000000              __bss_start
0000000000000000       F *UND*  0000000000000000              std::ios_base::Init::Init()@GLIBCXX_3.4
0000000000004150 g     O .bss   0000000000000004              global_var_in_dso
0000000000000000       F *UND*  0000000000000000              std::ostream::operator<<(int)@GLIBCXX_3.4
0000000000000000  w      *UND*  0000000000000000              _ITM_deregisterTMCloneTable
0000000000000000  w      *UND*  0000000000000000              __gmon_start__
0000000000000000  w      *UND*  0000000000000000              _ITM_registerTMCloneTable
0000000000000000       F *UND*  0000000000000000              std::ios_base::Init::~Init()@GLIBCXX_3.4
```

### -fPIC and -fpic, GOT and PLT

From [tldp.org](https://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html): 
> Use -fPIC or -fpic to generate code. Whether to use -fPIC or -fpic to generate code is target-dependent. The -fPIC choice always works, but may produce larger code than -fpic (mnenomic to remember this is that PIC is in a larger case, so it may produce larger amounts of code). Using -fpic option usually generates smaller and faster code, but will have platform-dependent limitations, such as the number of globally visible symbols or the size of the code. The linker will tell you whether it fits when you create the shared library. When in doubt, I choose -fPIC, because it always works.

Global Offset Table is used when we compile the objects with `-fPIC/-fpic` flag, and is filled by dynamic linker at runtime. Each relocatable symbol gets entry inside GOT, and this entry address is known during link time (i.e. static linking, not runtime). Dynamic linker then populates the entry when performs symbol lookup.

PLT, or Procedure Linkage Table, is a similar concept used for externally linked functions. While the GOT is modified, however, PLT is *sometimes* not and hence *may be* located in a read-only segment. Actual address of the relocated function in case of read-only PLT is saved in GOT again, where each function has corresponding entry which can be found from the PLT table.

When we do not use `-fPIC/-fpic`, we end up with relocations being written right into `text` segment, because in this case the code may contain absolute addresses for global variables and functions, which will have to be modified during startup by dynamic linker. This is bad, because it prevents multiple running executables using this binary object (compiled without `fpic`) from sharing the pages contining mapped code.

It is possible to see if a binary object contains text relocations via command:
```
readelf -d binary | grep TEXTREL
```
