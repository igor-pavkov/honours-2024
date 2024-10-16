static void vmx_flush_tlb(struct kvm_vcpu *vcpu)
{
	vpid_sync_vcpu_all(to_vmx(vcpu));
	if (vm_need_ept())
		ept_sync_context(construct_eptp(vcpu->arch.mmu.root_hpa));
}