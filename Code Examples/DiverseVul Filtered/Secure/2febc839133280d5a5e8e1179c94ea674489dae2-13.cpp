static s64 kpit_elapsed(struct kvm *kvm, struct kvm_kpit_channel_state *c,
			int channel)
{
	if (channel == 0)
		return __kpit_elapsed(kvm);

	return ktime_to_ns(ktime_sub(ktime_get(), c->count_load_time));
}