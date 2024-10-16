void __kvm_request_immediate_exit(struct kvm_vcpu *vcpu)
{
	smp_send_reschedule(vcpu->cpu);
}