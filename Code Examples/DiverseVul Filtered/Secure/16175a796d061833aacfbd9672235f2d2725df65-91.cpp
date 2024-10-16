static int vmx_get_irq(struct kvm_vcpu *vcpu)
{
	if (!vcpu->arch.interrupt.pending)
		return -1;
	return vcpu->arch.interrupt.nr;
}