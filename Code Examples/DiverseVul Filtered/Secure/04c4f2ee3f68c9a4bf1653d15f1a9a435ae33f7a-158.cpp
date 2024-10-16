static void vmx_post_block(struct kvm_vcpu *vcpu)
{
	if (kvm_x86_ops.set_hv_timer)
		kvm_lapic_switch_to_hv_timer(vcpu);

	pi_post_block(vcpu);
}