static void vmx_flush_tlb_current(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	u64 root_hpa = mmu->root_hpa;

	/* No flush required if the current context is invalid. */
	if (!VALID_PAGE(root_hpa))
		return;

	if (enable_ept)
		ept_sync_context(construct_eptp(vcpu, root_hpa,
						mmu->shadow_root_level));
	else if (!is_guest_mode(vcpu))
		vpid_sync_context(to_vmx(vcpu)->vpid);
	else
		vpid_sync_context(nested_get_vpid02(vcpu));
}