static void vmx_flush_tlb_all(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * INVEPT must be issued when EPT is enabled, irrespective of VPID, as
	 * the CPU is not required to invalidate guest-physical mappings on
	 * VM-Entry, even if VPID is disabled.  Guest-physical mappings are
	 * associated with the root EPT structure and not any particular VPID
	 * (INVVPID also isn't required to invalidate guest-physical mappings).
	 */
	if (enable_ept) {
		ept_sync_global();
	} else if (enable_vpid) {
		if (cpu_has_vmx_invvpid_global()) {
			vpid_sync_vcpu_global();
		} else {
			vpid_sync_vcpu_single(vmx->vpid);
			vpid_sync_vcpu_single(vmx->nested.vpid02);
		}
	}
}