#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include <elf.h>

int main() {
	std::ifstream f("./hello", std::ios::binary);
	const std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(f), {});
	const Elf64_Ehdr& elf_header = *reinterpret_cast<const Elf64_Ehdr*>(buffer.data());

	assert(elf_header.e_ident[EI_MAG0] == ELFMAG0);
	assert(elf_header.e_ident[EI_MAG1] == ELFMAG1);
	assert(elf_header.e_ident[EI_MAG2] == ELFMAG2);
	assert(elf_header.e_ident[EI_MAG3] == ELFMAG3);

	if (elf_header.e_ident[EI_CLASS] == ELFCLASS32) {
		std::cout << "Elf file is built for 32-bit architecture\n";
	} else if (elf_header.e_ident[EI_CLASS] == ELFCLASS64) {
		std::cout << "Elf file is built for 64-bit architecture\n";
	} else {
		std::cout << "Unknown architecture!\n";
	}
	return 0;
}
