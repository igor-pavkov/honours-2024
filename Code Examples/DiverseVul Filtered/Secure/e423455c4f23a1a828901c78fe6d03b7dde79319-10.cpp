void *rom_ptr(hwaddr addr, size_t size)
{
    Rom *rom;

    rom = find_rom(addr, size);
    if (!rom || !rom->data)
        return NULL;
    return rom->data + (addr - rom->addr);
}