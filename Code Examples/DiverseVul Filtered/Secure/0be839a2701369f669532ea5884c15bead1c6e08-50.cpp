void pci_register_soundhw(const char *name, const char *descr,
                          int (*init_pci)(PCIBus *bus))
{
    assert(soundhw_count < ARRAY_SIZE(soundhw) - 1);
    soundhw[soundhw_count].name = name;
    soundhw[soundhw_count].descr = descr;
    soundhw[soundhw_count].isa = 0;
    soundhw[soundhw_count].init.init_pci = init_pci;
    soundhw_count++;
}