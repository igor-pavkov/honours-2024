void nested_vmx_free_vcpu(struct kvm_vcpu *vcpu)
{
	vcpu_load(vcpu);
	vmx_leave_nested(vcpu);
	vmx_switch_vmcs(vcpu, &to_vmx(vcpu)->vmcs01);
	free_nested(vcpu);
	vcpu_put(vcpu);
}