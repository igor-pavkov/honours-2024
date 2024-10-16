static void e1000_write_config(PCIDevice *pci_dev, uint32_t address,
                                uint32_t val, int len)
{
    E1000State *s = E1000(pci_dev);

    pci_default_write_config(pci_dev, address, val, len);

    if (range_covers_byte(address, len, PCI_COMMAND) &&
        (pci_dev->config[PCI_COMMAND] & PCI_COMMAND_MASTER)) {
        qemu_flush_queued_packets(qemu_get_queue(s->nic));
    }
}