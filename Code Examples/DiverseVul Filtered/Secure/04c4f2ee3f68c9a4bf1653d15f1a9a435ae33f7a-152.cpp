void ept_save_pdptrs(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.walk_mmu;

	if (WARN_ON_ONCE(!is_pae_paging(vcpu)))
		return;

	mmu->pdptrs[0] = vmcs_read64(GUEST_PDPTR0);
	mmu->pdptrs[1] = vmcs_read64(GUEST_PDPTR1);
	mmu->pdptrs[2] = vmcs_read64(GUEST_PDPTR2);
	mmu->pdptrs[3] = vmcs_read64(GUEST_PDPTR3);

	kvm_register_mark_dirty(vcpu, VCPU_EXREG_PDPTR);
}