static int init_rmode_identity_map(struct kvm *kvm)
{
	struct kvm_vmx *kvm_vmx = to_kvm_vmx(kvm);
	int i, r = 0;
	void __user *uaddr;
	u32 tmp;

	/* Protect kvm_vmx->ept_identity_pagetable_done. */
	mutex_lock(&kvm->slots_lock);

	if (likely(kvm_vmx->ept_identity_pagetable_done))
		goto out;

	if (!kvm_vmx->ept_identity_map_addr)
		kvm_vmx->ept_identity_map_addr = VMX_EPT_IDENTITY_PAGETABLE_ADDR;

	uaddr = __x86_set_memory_region(kvm,
					IDENTITY_PAGETABLE_PRIVATE_MEMSLOT,
					kvm_vmx->ept_identity_map_addr,
					PAGE_SIZE);
	if (IS_ERR(uaddr)) {
		r = PTR_ERR(uaddr);
		goto out;
	}

	/* Set up identity-mapping pagetable for EPT in real mode */
	for (i = 0; i < PT32_ENT_PER_PAGE; i++) {
		tmp = (i << 22) + (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER |
			_PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_PSE);
		if (__copy_to_user(uaddr + i * sizeof(tmp), &tmp, sizeof(tmp))) {
			r = -EFAULT;
			goto out;
		}
	}
	kvm_vmx->ept_identity_pagetable_done = true;

out:
	mutex_unlock(&kvm->slots_lock);
	return r;
}