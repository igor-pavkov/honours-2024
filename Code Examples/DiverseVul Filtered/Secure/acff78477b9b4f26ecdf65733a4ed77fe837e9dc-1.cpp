static int nested_vmx_load_cr3(struct kvm_vcpu *vcpu, unsigned long cr3, bool nested_ept,
			       u32 *entry_failure_code)
{
	if (cr3 != kvm_read_cr3(vcpu) || (!nested_ept && pdptrs_changed(vcpu))) {
		if (!nested_cr3_valid(vcpu, cr3)) {
			*entry_failure_code = ENTRY_FAIL_DEFAULT;
			return 1;
		}

		/*
		 * If PAE paging and EPT are both on, CR3 is not used by the CPU and
		 * must not be dereferenced.
		 */
		if (!is_long_mode(vcpu) && is_pae(vcpu) && is_paging(vcpu) &&
		    !nested_ept) {
			if (!load_pdptrs(vcpu, vcpu->arch.walk_mmu, cr3)) {
				*entry_failure_code = ENTRY_FAIL_PDPTE;
				return 1;
			}
		}
	}

	if (!nested_ept)
		kvm_mmu_new_cr3(vcpu, cr3, false);

	vcpu->arch.cr3 = cr3;
	__set_bit(VCPU_EXREG_CR3, (ulong *)&vcpu->arch.regs_avail);

	kvm_init_mmu(vcpu, false);

	return 0;
}