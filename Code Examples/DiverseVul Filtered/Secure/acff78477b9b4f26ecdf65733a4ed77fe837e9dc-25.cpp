static int handle_vmoff(struct kvm_vcpu *vcpu)
{
	if (!nested_vmx_check_permission(vcpu))
		return 1;
	free_nested(vcpu);
	return nested_vmx_succeed(vcpu);
}