static int vmx_set_hv_timer(struct kvm_vcpu *vcpu, u64 guest_deadline_tsc,
			    bool *expired)
{
	struct vcpu_vmx *vmx;
	u64 tscl, guest_tscl, delta_tsc, lapic_timer_advance_cycles;
	struct kvm_timer *ktimer = &vcpu->arch.apic->lapic_timer;

	vmx = to_vmx(vcpu);
	tscl = rdtsc();
	guest_tscl = kvm_read_l1_tsc(vcpu, tscl);
	delta_tsc = max(guest_deadline_tsc, guest_tscl) - guest_tscl;
	lapic_timer_advance_cycles = nsec_to_cycles(vcpu,
						    ktimer->timer_advance_ns);

	if (delta_tsc > lapic_timer_advance_cycles)
		delta_tsc -= lapic_timer_advance_cycles;
	else
		delta_tsc = 0;

	/* Convert to host delta tsc if tsc scaling is enabled */
	if (vcpu->arch.tsc_scaling_ratio != kvm_default_tsc_scaling_ratio &&
	    delta_tsc && u64_shl_div_u64(delta_tsc,
				kvm_tsc_scaling_ratio_frac_bits,
				vcpu->arch.tsc_scaling_ratio, &delta_tsc))
		return -ERANGE;

	/*
	 * If the delta tsc can't fit in the 32 bit after the multi shift,
	 * we can't use the preemption timer.
	 * It's possible that it fits on later vmentries, but checking
	 * on every vmentry is costly so we just use an hrtimer.
	 */
	if (delta_tsc >> (cpu_preemption_timer_multi + 32))
		return -ERANGE;

	vmx->hv_deadline_tsc = tscl + delta_tsc;
	*expired = !delta_tsc;
	return 0;
}