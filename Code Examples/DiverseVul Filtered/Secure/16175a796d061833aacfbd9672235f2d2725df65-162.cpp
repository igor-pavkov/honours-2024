static void vmx_set_cr3(struct kvm_vcpu *vcpu, unsigned long cr3)
{
	unsigned long guest_cr3;
	u64 eptp;

	guest_cr3 = cr3;
	if (vm_need_ept()) {
		eptp = construct_eptp(cr3);
		vmcs_write64(EPT_POINTER, eptp);
		ept_sync_context(eptp);
		ept_load_pdptrs(vcpu);
		guest_cr3 = is_paging(vcpu) ? vcpu->arch.cr3 :
			VMX_EPT_IDENTITY_PAGETABLE_ADDR;
	}

	vmx_flush_tlb(vcpu);
	vmcs_writel(GUEST_CR3, guest_cr3);
	if (vcpu->arch.cr0 & X86_CR0_PE)
		vmx_fpu_deactivate(vcpu);
}