int kvm_read_guest_page(struct kvm *kvm, gfn_t gfn, void *data, int offset,
			int len)
{
	int r;
	unsigned long addr;

	addr = gfn_to_hva_read(kvm, gfn);
	if (kvm_is_error_hva(addr))
		return -EFAULT;
	r = kvm_read_hva(data, (void __user *)addr + offset, len);
	if (r)
		return -EFAULT;
	return 0;
}