static void vmx_flush_tlb_guest(struct kvm_vcpu *vcpu)
{
	/*
	 * vpid_sync_context() is a nop if vmx->vpid==0, e.g. if enable_vpid==0
	 * or a vpid couldn't be allocated for this vCPU.  VM-Enter and VM-Exit
	 * are required to flush GVA->{G,H}PA mappings from the TLB if vpid is
	 * disabled (VM-Enter with vpid enabled and vpid==0 is disallowed),
	 * i.e. no explicit INVVPID is necessary.
	 */
	vpid_sync_context(to_vmx(vcpu)->vpid);
}