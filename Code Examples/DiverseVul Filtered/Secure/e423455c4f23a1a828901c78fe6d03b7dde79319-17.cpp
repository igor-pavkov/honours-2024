int rom_add_vga(const char *file)
{
    return rom_add_file(file, "vgaroms", 0, -1, true, NULL, NULL);
}