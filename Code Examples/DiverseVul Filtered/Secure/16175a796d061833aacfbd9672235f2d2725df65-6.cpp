static void kvm_do_inject_irq(struct kvm_vcpu *vcpu)
{
	int word_index = __ffs(vcpu->arch.irq_summary);
	int bit_index = __ffs(vcpu->arch.irq_pending[word_index]);
	int irq = word_index * BITS_PER_LONG + bit_index;

	clear_bit(bit_index, &vcpu->arch.irq_pending[word_index]);
	if (!vcpu->arch.irq_pending[word_index])
		clear_bit(word_index, &vcpu->arch.irq_summary);
	kvm_queue_interrupt(vcpu, irq);
}