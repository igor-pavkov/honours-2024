static int handle_wbinvd(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	skip_emulated_instruction(vcpu);
	/* TODO: Add support for VT-d/pass-through device */
	return 1;
}