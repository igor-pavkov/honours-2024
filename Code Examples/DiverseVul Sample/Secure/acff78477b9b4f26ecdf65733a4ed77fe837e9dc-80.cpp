static int nested_vmx_check_pml_controls(struct kvm_vcpu *vcpu,
					 struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has_pml(vmcs12))
		return 0;

	if (!nested_cpu_has_ept(vmcs12) ||
	    !page_address_valid(vcpu, vmcs12->pml_address))
		return -EINVAL;

	return 0;
}