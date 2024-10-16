static int nested_vmx_check_shadow_vmcs_controls(struct kvm_vcpu *vcpu,
						 struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has_shadow_vmcs(vmcs12))
		return 0;

	if (!page_address_valid(vcpu, vmcs12->vmread_bitmap) ||
	    !page_address_valid(vcpu, vmcs12->vmwrite_bitmap))
		return -EINVAL;

	return 0;
}