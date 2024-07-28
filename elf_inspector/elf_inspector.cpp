#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include <elf.h>

#include <boost/algorithm/string/join.hpp>

void print_architecture(const Elf64_Ehdr& elf_header) {
  if (elf_header.e_ident[EI_CLASS] == ELFCLASS32) {
    std::cout << "Elf file is built for 32-bit architecture\n";
  } else if (elf_header.e_ident[EI_CLASS] == ELFCLASS64) {
    std::cout << "Elf file is built for 64-bit architecture\n";
  } else {
    std::cout << "Unknown architecture!\n";
  }
}

void print_object_file_type(const Elf64_Ehdr& elf_header) {
  std::cout << "Object file type: ";
  switch (elf_header.e_type) {
  case ET_NONE: {
    std::cout << "unknown\n";
    break;
  }
  case ET_REL: {
    std::cout << "relocatable file\n";
    break;
  }
  case ET_EXEC: {
    std::cout << "executable file\n";
    break;
  }
  case ET_DYN: {
    std::cout << "shared object\n";
    break;
  }
  case ET_CORE: {
    std::cout << "core file\n";
    break;
  }
  default: {
    std::cout << "Unexpected file type!\n";
    assert(false);
  }
  }
}

void print_program_header_table(const Elf64_Phdr* phdr_table, Elf64_Half size, 
    const std::vector<char>& buffer) {
  for (Elf64_Half i = 0; i < size; i++) {
    std::cout << "Program segment #" << i << " header:\n";
    std::cout << "  Segment kind: ";

    if (phdr_table[i].p_type >= PT_LOPROC && phdr_table[i].p_type <= PT_HIPROC) {
      std::cout << "reserved for processor-specific semantics\n";
      continue;
    }
    switch (phdr_table[i].p_type) {
    case PT_NULL: {
      std::cout << "[PT_NULL] unused\n";
      break;
    }
    case PT_LOAD: {
      std::cout << "[PT_LOAD] loadable segment\n  flags: ";
      std::vector<std::string> flags;
      if (phdr_table[i].p_flags & PF_X) {
        flags.push_back("EXEC");
      }
      if (phdr_table[i].p_flags & PF_W) {
        flags.push_back("WRITE");
      }
      if (phdr_table[i].p_flags & PF_R) {
        flags.push_back("READ");
      }

      std::cout << boost::algorithm::join(flags, "|") << "\n";

      break;
    }
    case PT_DYNAMIC: {
      std::cout << "[PT_DYNAMIC] dynamic linking information\n";
      break;
    }
    case PT_INTERP: {
      std::cout << "[PT_INTERP] location and size of a null-terminated pathname to invoke as an interpreter\n";
      std::string_view interp_str(buffer.data() + phdr_table[i].p_offset, phdr_table[i].p_filesz);
      std::cout << "  interpreter: " << interp_str << "\n";
      break;
    }
    case PT_PHDR: {
      std::cout << "[PT_PHDR] location and size of the program header table itself\n";
      break;
    }
    case PT_NOTE: {
      std::cout << "[PT_NOTE] The array element specifies the location of notes\n";
      break;
    }
    case PT_GNU_STACK: {
      std::cout << "[PT_GNU_STACK] GNU extension which is used by the Linux kernel "
        "to control the state of the stack via the flags set in the p_flags member\n";
      break;
    }
    default: {
      std::cout << "some other segment\n";
      break;
    }
    }
  }
}


int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Expected inspected file as first argument\n";
    exit(1);
  }

  std::ifstream f(argv[1], std::ios::binary);

  if (!f) {
    std::cerr << "Failed to open inspected file\n";
    exit(1);
  }
  const std::vector<char> buffer(std::istreambuf_iterator<char>(f), {});
  const Elf64_Ehdr& elf_header = *reinterpret_cast<const Elf64_Ehdr*>(buffer.data());

  assert(elf_header.e_ident[EI_MAG0] == ELFMAG0);
  assert(elf_header.e_ident[EI_MAG1] == ELFMAG1);
  assert(elf_header.e_ident[EI_MAG2] == ELFMAG2);
  assert(elf_header.e_ident[EI_MAG3] == ELFMAG3);

  print_architecture(elf_header);
  print_object_file_type(elf_header);

  std::cout << "entry point virtual address: 0x" << std::hex << elf_header.e_entry << "\n";
  std::cout << "Number of entries in program header table: " << std::dec << elf_header.e_phnum << "\n";
  if (elf_header.e_phnum > 0) {
    print_program_header_table(reinterpret_cast<const Elf64_Phdr*>(buffer.data() + elf_header.e_phoff), 
      elf_header.e_phnum,
      buffer);
  }
  return 0;
}
