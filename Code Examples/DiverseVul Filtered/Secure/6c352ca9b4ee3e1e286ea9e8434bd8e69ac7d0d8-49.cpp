static bool vmxnet3_vmstate_need_pcie_device(void *opaque)
{
    VMXNET3State *s = VMXNET3(opaque);

    return !(s->compat_flags & VMXNET3_COMPAT_FLAG_DISABLE_PCIE);
}