static bool kvm_propagate_fault(struct kvm_vcpu *vcpu, struct x86_exception *fault)
{
	if (mmu_is_nested(vcpu) && !fault->nested_page_fault)
		vcpu->arch.nested_mmu.inject_page_fault(vcpu, fault);
	else
		vcpu->arch.mmu->inject_page_fault(vcpu, fault);

	return fault->nested_page_fault;
}