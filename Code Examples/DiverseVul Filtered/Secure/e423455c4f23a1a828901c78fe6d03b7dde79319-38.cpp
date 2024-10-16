int load_elf_ram(const char *filename,
                 uint64_t (*elf_note_fn)(void *, void *, bool),
                 uint64_t (*translate_fn)(void *, uint64_t),
                 void *translate_opaque, uint64_t *pentry, uint64_t *lowaddr,
                 uint64_t *highaddr, int big_endian, int elf_machine,
                 int clear_lsb, int data_swab, AddressSpace *as,
                 bool load_rom)
{
    return load_elf_ram_sym(filename, elf_note_fn,
                            translate_fn, translate_opaque,
                            pentry, lowaddr, highaddr, big_endian,
                            elf_machine, clear_lsb, data_swab, as,
                            load_rom, NULL);
}