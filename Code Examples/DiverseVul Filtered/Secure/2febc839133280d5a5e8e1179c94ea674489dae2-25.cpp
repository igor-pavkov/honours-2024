static int pit_get_gate(struct kvm *kvm, int channel)
{
	WARN_ON(!mutex_is_locked(&kvm->arch.vpit->pit_state.lock));

	return kvm->arch.vpit->pit_state.channels[channel].gate;
}