static int speaker_ioport_write(struct kvm_io_device *this,
				gpa_t addr, int len, const void *data)
{
	struct kvm_pit *pit = speaker_to_pit(this);
	struct kvm_kpit_state *pit_state = &pit->pit_state;
	struct kvm *kvm = pit->kvm;
	u32 val = *(u32 *) data;
	if (addr != KVM_SPEAKER_BASE_ADDRESS)
		return -EOPNOTSUPP;

	mutex_lock(&pit_state->lock);
	pit_state->speaker_data_on = (val >> 1) & 1;
	pit_set_gate(kvm, 2, val & 1);
	mutex_unlock(&pit_state->lock);
	return 0;
}