static int speaker_ioport_read(struct kvm_io_device *this,
			       gpa_t addr, int len, void *data)
{
	struct kvm_pit *pit = speaker_to_pit(this);
	struct kvm_kpit_state *pit_state = &pit->pit_state;
	struct kvm *kvm = pit->kvm;
	unsigned int refresh_clock;
	int ret;
	if (addr != KVM_SPEAKER_BASE_ADDRESS)
		return -EOPNOTSUPP;

	/* Refresh clock toggles at about 15us. We approximate as 2^14ns. */
	refresh_clock = ((unsigned int)ktime_to_ns(ktime_get()) >> 14) & 1;

	mutex_lock(&pit_state->lock);
	ret = ((pit_state->speaker_data_on << 1) | pit_get_gate(kvm, 2) |
		(pit_get_out(kvm, 2) << 5) | (refresh_clock << 4));
	if (len > sizeof(ret))
		len = sizeof(ret);
	memcpy(data, (char *)&ret, len);
	mutex_unlock(&pit_state->lock);
	return 0;
}