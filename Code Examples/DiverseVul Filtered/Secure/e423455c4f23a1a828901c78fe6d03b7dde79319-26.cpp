static void rom_free_data(Rom *rom)
{
    if (rom->mapped_file) {
        g_mapped_file_unref(rom->mapped_file);
        rom->mapped_file = NULL;
    } else {
        g_free(rom->data);
    }

    rom->data = NULL;
}