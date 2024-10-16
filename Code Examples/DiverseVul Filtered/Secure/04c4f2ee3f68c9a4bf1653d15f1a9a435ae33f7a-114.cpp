static void vmx_migrate_timers(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		struct hrtimer *timer = &to_vmx(vcpu)->nested.preemption_timer;

		if (hrtimer_try_to_cancel(timer) == 1)
			hrtimer_start_expires(timer, HRTIMER_MODE_ABS_PINNED);
	}
}