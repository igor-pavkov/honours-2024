static void vmx_flush_tlb_gva(struct kvm_vcpu *vcpu, gva_t addr)
{
	/*
	 * vpid_sync_vcpu_addr() is a nop if vmx->vpid==0, see the comment in
	 * vmx_flush_tlb_guest() for an explanation of why this is ok.
	 */
	vpid_sync_vcpu_addr(to_vmx(vcpu)->vpid, addr);
}