void kvm_queue_exception(struct kvm_vcpu *vcpu, unsigned nr)
{
	WARN_ON(vcpu->arch.exception.pending);
	vcpu->arch.exception.pending = true;
	vcpu->arch.exception.has_error_code = false;
	vcpu->arch.exception.nr = nr;
}