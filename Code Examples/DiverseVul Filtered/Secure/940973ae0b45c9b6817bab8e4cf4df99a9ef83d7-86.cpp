void ide_init2(IDEBus *bus, qemu_irq irq)
{
    int i;

    for(i = 0; i < 2; i++) {
        ide_init1(bus, i);
        ide_reset(&bus->ifs[i]);
    }
    bus->irq = irq;
    bus->dma = &ide_dma_nop;
}