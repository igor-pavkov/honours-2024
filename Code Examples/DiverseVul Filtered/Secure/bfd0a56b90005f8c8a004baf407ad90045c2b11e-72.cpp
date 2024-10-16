static void ept_load_pdptrs(struct kvm_vcpu *vcpu)
{
	if (!test_bit(VCPU_EXREG_PDPTR,
		      (unsigned long *)&vcpu->arch.regs_dirty))
		return;

	if (is_paging(vcpu) && is_pae(vcpu) && !is_long_mode(vcpu)) {
		vmcs_write64(GUEST_PDPTR0, vcpu->arch.mmu.pdptrs[0]);
		vmcs_write64(GUEST_PDPTR1, vcpu->arch.mmu.pdptrs[1]);
		vmcs_write64(GUEST_PDPTR2, vcpu->arch.mmu.pdptrs[2]);
		vmcs_write64(GUEST_PDPTR3, vcpu->arch.mmu.pdptrs[3]);
	}
}