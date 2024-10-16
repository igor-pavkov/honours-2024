static void kvm_queue_exception_e_p(struct kvm_vcpu *vcpu, unsigned nr,
				    u32 error_code, unsigned long payload)
{
	kvm_multiple_exception(vcpu, nr, true, error_code,
			       true, payload, false);
}