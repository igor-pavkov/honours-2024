static void vmx_start_preemption_timer(struct kvm_vcpu *vcpu)
{
	u64 preemption_timeout = get_vmcs12(vcpu)->vmx_preemption_timer_value;
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * A timer value of zero is architecturally guaranteed to cause
	 * a VMExit prior to executing any instructions in the guest.
	 */
	if (preemption_timeout == 0) {
		vmx_preemption_timer_fn(&vmx->nested.preemption_timer);
		return;
	}

	if (vcpu->arch.virtual_tsc_khz == 0)
		return;

	preemption_timeout <<= VMX_MISC_EMULATED_PREEMPTION_TIMER_RATE;
	preemption_timeout *= 1000000;
	do_div(preemption_timeout, vcpu->arch.virtual_tsc_khz);
	hrtimer_start(&vmx->nested.preemption_timer,
		      ns_to_ktime(preemption_timeout), HRTIMER_MODE_REL);
}