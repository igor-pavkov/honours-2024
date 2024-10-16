static void rom_free(Rom *rom)
{
    rom_free_data(rom);
    g_free(rom->path);
    g_free(rom->name);
    g_free(rom->fw_dir);
    g_free(rom->fw_file);
    g_free(rom);
}