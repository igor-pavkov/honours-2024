static int kvm_dev_ioctl_create_vm(unsigned long type)
{
	int r;
	struct kvm *kvm;

	kvm = kvm_create_vm(type);
	if (IS_ERR(kvm))
		return PTR_ERR(kvm);
#ifdef KVM_COALESCED_MMIO_PAGE_OFFSET
	r = kvm_coalesced_mmio_init(kvm);
	if (r < 0) {
		kvm_put_kvm(kvm);
		return r;
	}
#endif
	r = anon_inode_getfd("kvm-vm", &kvm_vm_fops, kvm, O_RDWR | O_CLOEXEC);
	if (r < 0)
		kvm_put_kvm(kvm);

	return r;
}