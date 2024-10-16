void kvm_pit_load_count(struct kvm *kvm, int channel, u32 val, int hpet_legacy_start)
{
	u8 saved_mode;
	if (hpet_legacy_start) {
		/* save existing mode for later reenablement */
		saved_mode = kvm->arch.vpit->pit_state.channels[0].mode;
		kvm->arch.vpit->pit_state.channels[0].mode = 0xff; /* disable timer */
		pit_load_count(kvm, channel, val);
		kvm->arch.vpit->pit_state.channels[0].mode = saved_mode;
	} else {
		pit_load_count(kvm, channel, val);
	}
}