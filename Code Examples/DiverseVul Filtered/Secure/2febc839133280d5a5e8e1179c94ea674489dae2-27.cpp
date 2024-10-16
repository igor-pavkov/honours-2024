static int pit_ioport_write(struct kvm_io_device *this,
			    gpa_t addr, int len, const void *data)
{
	struct kvm_pit *pit = dev_to_pit(this);
	struct kvm_kpit_state *pit_state = &pit->pit_state;
	struct kvm *kvm = pit->kvm;
	int channel, access;
	struct kvm_kpit_channel_state *s;
	u32 val = *(u32 *) data;
	if (!pit_in_range(addr))
		return -EOPNOTSUPP;

	val  &= 0xff;
	addr &= KVM_PIT_CHANNEL_MASK;

	mutex_lock(&pit_state->lock);

	if (val != 0)
		pr_debug("write addr is 0x%x, len is %d, val is 0x%x\n",
			 (unsigned int)addr, len, val);

	if (addr == 3) {
		channel = val >> 6;
		if (channel == 3) {
			/* Read-Back Command. */
			for (channel = 0; channel < 3; channel++) {
				s = &pit_state->channels[channel];
				if (val & (2 << channel)) {
					if (!(val & 0x20))
						pit_latch_count(kvm, channel);
					if (!(val & 0x10))
						pit_latch_status(kvm, channel);
				}
			}
		} else {
			/* Select Counter <channel>. */
			s = &pit_state->channels[channel];
			access = (val >> 4) & KVM_PIT_CHANNEL_MASK;
			if (access == 0) {
				pit_latch_count(kvm, channel);
			} else {
				s->rw_mode = access;
				s->read_state = access;
				s->write_state = access;
				s->mode = (val >> 1) & 7;
				if (s->mode > 5)
					s->mode -= 4;
				s->bcd = val & 1;
			}
		}
	} else {
		/* Write Count. */
		s = &pit_state->channels[addr];
		switch (s->write_state) {
		default:
		case RW_STATE_LSB:
			pit_load_count(kvm, addr, val);
			break;
		case RW_STATE_MSB:
			pit_load_count(kvm, addr, val << 8);
			break;
		case RW_STATE_WORD0:
			s->write_latch = val;
			s->write_state = RW_STATE_WORD1;
			break;
		case RW_STATE_WORD1:
			pit_load_count(kvm, addr, s->write_latch | (val << 8));
			s->write_state = RW_STATE_WORD0;
			break;
		}
	}

	mutex_unlock(&pit_state->lock);
	return 0;
}