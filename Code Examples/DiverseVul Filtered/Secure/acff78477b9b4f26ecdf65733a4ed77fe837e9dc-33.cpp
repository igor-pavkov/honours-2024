static void free_nested(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx->nested.vmxon && !vmx->nested.smm.vmxon)
		return;

	vmx->nested.vmxon = false;
	vmx->nested.smm.vmxon = false;
	free_vpid(vmx->nested.vpid02);
	vmx->nested.posted_intr_nv = -1;
	vmx->nested.current_vmptr = -1ull;
	if (enable_shadow_vmcs) {
		vmx_disable_shadow_vmcs(vmx);
		vmcs_clear(vmx->vmcs01.shadow_vmcs);
		free_vmcs(vmx->vmcs01.shadow_vmcs);
		vmx->vmcs01.shadow_vmcs = NULL;
	}
	kfree(vmx->nested.cached_vmcs12);
	kfree(vmx->nested.cached_shadow_vmcs12);
	/* Unpin physical memory we referred to in the vmcs02 */
	if (vmx->nested.apic_access_page) {
		kvm_release_page_dirty(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page = NULL;
	}
	if (vmx->nested.virtual_apic_page) {
		kvm_release_page_dirty(vmx->nested.virtual_apic_page);
		vmx->nested.virtual_apic_page = NULL;
	}
	if (vmx->nested.pi_desc_page) {
		kunmap(vmx->nested.pi_desc_page);
		kvm_release_page_dirty(vmx->nested.pi_desc_page);
		vmx->nested.pi_desc_page = NULL;
		vmx->nested.pi_desc = NULL;
	}

	kvm_mmu_free_roots(vcpu, &vcpu->arch.guest_mmu, KVM_MMU_ROOTS_ALL);

	nested_release_evmcs(vcpu);

	free_loaded_vmcs(&vmx->nested.vmcs02);
}