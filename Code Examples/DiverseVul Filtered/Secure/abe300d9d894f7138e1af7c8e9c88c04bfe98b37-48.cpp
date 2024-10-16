static int failover_set_primary(DeviceState *dev, void *opaque)
{
    FailoverDevice *fdev = opaque;
    PCIDevice *pci_dev = (PCIDevice *)
        object_dynamic_cast(OBJECT(dev), TYPE_PCI_DEVICE);

    if (!pci_dev) {
        return 0;
    }

    if (!g_strcmp0(pci_dev->failover_pair_id, fdev->n->netclient_name)) {
        fdev->dev = dev;
        return 1;
    }

    return 0;
}