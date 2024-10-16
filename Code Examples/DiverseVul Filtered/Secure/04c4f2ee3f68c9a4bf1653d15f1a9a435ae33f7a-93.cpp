static void vmx_load_mmu_pgd(struct kvm_vcpu *vcpu, unsigned long pgd,
			     int pgd_level)
{
	struct kvm *kvm = vcpu->kvm;
	bool update_guest_cr3 = true;
	unsigned long guest_cr3;
	u64 eptp;

	if (enable_ept) {
		eptp = construct_eptp(vcpu, pgd, pgd_level);
		vmcs_write64(EPT_POINTER, eptp);

		if (kvm_x86_ops.tlb_remote_flush) {
			spin_lock(&to_kvm_vmx(kvm)->ept_pointer_lock);
			to_vmx(vcpu)->ept_pointer = eptp;
			to_kvm_vmx(kvm)->ept_pointers_match
				= EPT_POINTERS_CHECK;
			spin_unlock(&to_kvm_vmx(kvm)->ept_pointer_lock);
		}

		if (!enable_unrestricted_guest && !is_paging(vcpu))
			guest_cr3 = to_kvm_vmx(kvm)->ept_identity_map_addr;
		else if (test_bit(VCPU_EXREG_CR3, (ulong *)&vcpu->arch.regs_avail))
			guest_cr3 = vcpu->arch.cr3;
		else /* vmcs01.GUEST_CR3 is already up-to-date. */
			update_guest_cr3 = false;
		vmx_ept_load_pdptrs(vcpu);
	} else {
		guest_cr3 = pgd;
	}

	if (update_guest_cr3)
		vmcs_writel(GUEST_CR3, guest_cr3);
}