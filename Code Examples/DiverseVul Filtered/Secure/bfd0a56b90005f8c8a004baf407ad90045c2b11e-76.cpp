static int nested_ept_init_mmu_context(struct kvm_vcpu *vcpu)
{
	int r = kvm_init_shadow_ept_mmu(vcpu, &vcpu->arch.mmu,
			nested_vmx_ept_caps & VMX_EPT_EXECUTE_ONLY_BIT);

	vcpu->arch.mmu.set_cr3           = vmx_set_cr3;
	vcpu->arch.mmu.get_cr3           = nested_ept_get_cr3;
	vcpu->arch.mmu.inject_page_fault = nested_ept_inject_page_fault;

	vcpu->arch.walk_mmu              = &vcpu->arch.nested_mmu;

	return r;
}