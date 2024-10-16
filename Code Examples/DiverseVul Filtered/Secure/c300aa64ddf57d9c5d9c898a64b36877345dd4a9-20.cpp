static int kvm_vm_ioctl_set_tss_addr(struct kvm *kvm, unsigned long addr)
{
	int ret;

	if (addr > (unsigned int)(-3 * PAGE_SIZE))
		return -EINVAL;
	ret = kvm_x86_ops->set_tss_addr(kvm, addr);
	return ret;
}