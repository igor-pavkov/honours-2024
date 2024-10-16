static void vmx_cancel_hv_timer(struct kvm_vcpu *vcpu)
{
	to_vmx(vcpu)->hv_deadline_tsc = -1;
}