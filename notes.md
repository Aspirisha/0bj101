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
* `STB_WEAK` - same as global, bu allows multiple definitions of the symbol: when combining multiple object files, linker will forbid having more than one `STB_GLOBAL` entry for the same symbol, while it is ok to have multiple weakly bound definitions and 1 globally bound definition.

From https://akkadia.org/drepper/dsohowto.pdf:
Note that a definition in a DSO being *weak* has no effects. Weak definitions only play a role in static linking, and we can have multiple global definitions of the same symbol in distinct DSOs linked dynamically to an executable/other dso. Side note - of course, we will still have problem if we link multiple object files / static libraries when *building* dso - important thing is whether multiple global defintions during linking appear in dynamic libraries or not.