int kvm_read_guest_atomic(struct kvm *kvm, gpa_t gpa, void *data,
			  unsigned long len)
{
	int r;
	unsigned long addr;
	gfn_t gfn = gpa >> PAGE_SHIFT;
	int offset = offset_in_page(gpa);

	addr = gfn_to_hva_prot(kvm, gfn, NULL);
	if (kvm_is_error_hva(addr))
		return -EFAULT;
	pagefault_disable();
	r = kvm_read_hva_atomic(data, (void __user *)addr + offset, len);
	pagefault_enable();
	if (r)
		return -EFAULT;
	return 0;
}