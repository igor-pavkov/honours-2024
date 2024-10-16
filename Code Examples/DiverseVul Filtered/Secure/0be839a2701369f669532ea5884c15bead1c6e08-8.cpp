void isa_register_soundhw(const char *name, const char *descr,
                          int (*init_isa)(ISABus *bus))
{
    assert(soundhw_count < ARRAY_SIZE(soundhw) - 1);
    soundhw[soundhw_count].name = name;
    soundhw[soundhw_count].descr = descr;
    soundhw[soundhw_count].isa = 1;
    soundhw[soundhw_count].init.init_isa = init_isa;
    soundhw_count++;
}