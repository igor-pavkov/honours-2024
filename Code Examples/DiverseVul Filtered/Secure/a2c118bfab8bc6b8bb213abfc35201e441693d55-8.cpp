bool kvm_ioapic_handles_vector(struct kvm *kvm, int vector)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;
	smp_rmb();
	return test_bit(vector, ioapic->handled_vectors);
}