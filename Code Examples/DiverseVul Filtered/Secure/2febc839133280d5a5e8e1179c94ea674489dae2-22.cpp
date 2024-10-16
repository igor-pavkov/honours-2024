static void pit_set_gate(struct kvm *kvm, int channel, u32 val)
{
	struct kvm_kpit_channel_state *c =
		&kvm->arch.vpit->pit_state.channels[channel];

	WARN_ON(!mutex_is_locked(&kvm->arch.vpit->pit_state.lock));

	switch (c->mode) {
	default:
	case 0:
	case 4:
		/* XXX: just disable/enable counting */
		break;
	case 1:
	case 2:
	case 3:
	case 5:
		/* Restart counting on rising edge. */
		if (c->gate < val)
			c->count_load_time = ktime_get();
		break;
	}

	c->gate = val;
}