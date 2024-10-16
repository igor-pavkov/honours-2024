static void vmx_request_immediate_exit(struct kvm_vcpu *vcpu)
{
	to_vmx(vcpu)->req_immediate_exit = true;
}