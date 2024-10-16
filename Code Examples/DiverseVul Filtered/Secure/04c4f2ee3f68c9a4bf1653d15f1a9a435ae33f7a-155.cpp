static int handle_machine_check(struct kvm_vcpu *vcpu)
{
	/* handled by vmx_vcpu_run() */
	return 1;
}