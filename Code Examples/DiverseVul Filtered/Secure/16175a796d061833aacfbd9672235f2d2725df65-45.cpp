static void vmx_vcpu_put(struct kvm_vcpu *vcpu)
{
	__vmx_load_host_state(to_vmx(vcpu));
}