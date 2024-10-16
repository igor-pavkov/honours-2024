static void pit_latch_count(struct kvm *kvm, int channel)
{
	struct kvm_kpit_channel_state *c =
		&kvm->arch.vpit->pit_state.channels[channel];

	WARN_ON(!mutex_is_locked(&kvm->arch.vpit->pit_state.lock));

	if (!c->count_latched) {
		c->latched_count = pit_get_count(kvm, channel);
		c->count_latched = c->rw_mode;
	}
}