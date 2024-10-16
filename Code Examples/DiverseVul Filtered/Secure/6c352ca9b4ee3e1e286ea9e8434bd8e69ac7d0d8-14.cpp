static bool vmxnet3_vmstate_test_pci_device(void *opaque, int version_id)
{
    return !vmxnet3_vmstate_need_pcie_device(opaque);
}