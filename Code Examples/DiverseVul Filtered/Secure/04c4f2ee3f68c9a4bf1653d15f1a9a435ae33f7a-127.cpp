static int vmx_set_tss_addr(struct kvm *kvm, unsigned int addr)
{
	void __user *ret;

	if (enable_unrestricted_guest)
		return 0;

	mutex_lock(&kvm->slots_lock);
	ret = __x86_set_memory_region(kvm, TSS_PRIVATE_MEMSLOT, addr,
				      PAGE_SIZE * 3);
	mutex_unlock(&kvm->slots_lock);

	if (IS_ERR(ret))
		return PTR_ERR(ret);

	to_kvm_vmx(kvm)->tss_addr = addr;

	return init_rmode_tss(kvm, ret);
}