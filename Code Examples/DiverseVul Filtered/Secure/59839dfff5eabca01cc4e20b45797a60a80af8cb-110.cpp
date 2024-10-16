static void __queue_exception(struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->queue_exception(vcpu, vcpu->arch.exception.nr,
				     vcpu->arch.exception.has_error_code,
				     vcpu->arch.exception.error_code);
}