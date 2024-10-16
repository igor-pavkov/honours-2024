static void *rom_set_mr(Rom *rom, Object *owner, const char *name, bool ro)
{
    void *data;

    rom->mr = g_malloc(sizeof(*rom->mr));
    memory_region_init_resizeable_ram(rom->mr, owner, name,
                                      rom->datasize, rom->romsize,
                                      fw_cfg_resized,
                                      &error_fatal);
    memory_region_set_readonly(rom->mr, ro);
    vmstate_register_ram_global(rom->mr);

    data = memory_region_get_ram_ptr(rom->mr);
    memcpy(data, rom->data, rom->datasize);

    return data;
}