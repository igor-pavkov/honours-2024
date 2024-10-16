static bool nested_has_guest_tlb_tag(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	return nested_cpu_has_ept(vmcs12) ||
	       (nested_cpu_has_vpid(vmcs12) && to_vmx(vcpu)->nested.vpid02);
}