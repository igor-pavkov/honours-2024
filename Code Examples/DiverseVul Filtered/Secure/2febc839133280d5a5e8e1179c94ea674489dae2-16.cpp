static int pit_get_out(struct kvm *kvm, int channel)
{
	struct kvm_kpit_channel_state *c =
		&kvm->arch.vpit->pit_state.channels[channel];
	s64 d, t;
	int out;

	WARN_ON(!mutex_is_locked(&kvm->arch.vpit->pit_state.lock));

	t = kpit_elapsed(kvm, c, channel);
	d = muldiv64(t, KVM_PIT_FREQ, NSEC_PER_SEC);

	switch (c->mode) {
	default:
	case 0:
		out = (d >= c->count);
		break;
	case 1:
		out = (d < c->count);
		break;
	case 2:
		out = ((mod_64(d, c->count) == 0) && (d != 0));
		break;
	case 3:
		out = (mod_64(d, c->count) < ((c->count + 1) >> 1));
		break;
	case 4:
	case 5:
		out = (d == c->count);
		break;
	}

	return out;
}