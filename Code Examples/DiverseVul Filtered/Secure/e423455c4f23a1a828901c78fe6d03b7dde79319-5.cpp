int rom_add_elf_program(const char *name, GMappedFile *mapped_file, void *data,
                        size_t datasize, size_t romsize, hwaddr addr,
                        AddressSpace *as)
{
    Rom *rom;

    rom           = g_malloc0(sizeof(*rom));
    rom->name     = g_strdup(name);
    rom->addr     = addr;
    rom->datasize = datasize;
    rom->romsize  = romsize;
    rom->data     = data;
    rom->as       = as;

    if (mapped_file && data) {
        g_mapped_file_ref(mapped_file);
        rom->mapped_file = mapped_file;
    }

    rom_insert(rom);
    return 0;
}