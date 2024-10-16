static void kvm_queue_exception_p(struct kvm_vcpu *vcpu, unsigned nr,
				  unsigned long payload)
{
	kvm_multiple_exception(vcpu, nr, false, 0, true, payload, false);
}