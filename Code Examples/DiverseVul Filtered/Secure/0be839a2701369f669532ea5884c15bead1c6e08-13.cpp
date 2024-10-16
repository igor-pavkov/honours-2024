void audio_init(void)
{
    struct soundhw *c;
    ISABus *isa_bus = (ISABus *) object_resolve_path_type("", TYPE_ISA_BUS, NULL);
    PCIBus *pci_bus = (PCIBus *) object_resolve_path_type("", TYPE_PCI_BUS, NULL);

    for (c = soundhw; c->name; ++c) {
        if (c->enabled) {
            if (c->isa) {
                if (!isa_bus) {
                    error_report("ISA bus not available for %s", c->name);
                    exit(1);
                }
                c->init.init_isa(isa_bus);
            } else {
                if (!pci_bus) {
                    error_report("PCI bus not available for %s", c->name);
                    exit(1);
                }
                c->init.init_pci(pci_bus);
            }
        }
    }
}