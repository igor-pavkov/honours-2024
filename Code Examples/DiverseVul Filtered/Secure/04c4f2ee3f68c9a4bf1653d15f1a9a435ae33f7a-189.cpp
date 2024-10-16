static int handle_encls(struct kvm_vcpu *vcpu)
{
	/*
	 * SGX virtualization is not yet supported.  There is no software
	 * enable bit for SGX, so we have to trap ENCLS and inject a #UD
	 * to prevent the guest from executing ENCLS.
	 */
	kvm_queue_exception(vcpu, UD_VECTOR);
	return 1;
}