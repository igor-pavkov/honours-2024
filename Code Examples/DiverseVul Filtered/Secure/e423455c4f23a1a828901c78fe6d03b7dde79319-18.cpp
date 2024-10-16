int load_elf_as(const char *filename,
                uint64_t (*elf_note_fn)(void *, void *, bool),
                uint64_t (*translate_fn)(void *, uint64_t),
                void *translate_opaque, uint64_t *pentry, uint64_t *lowaddr,
                uint64_t *highaddr, int big_endian, int elf_machine,
                int clear_lsb, int data_swab, AddressSpace *as)
{
    return load_elf_ram(filename, elf_note_fn, translate_fn, translate_opaque,
                        pentry, lowaddr, highaddr, big_endian, elf_machine,
                        clear_lsb, data_swab, as, true);
}