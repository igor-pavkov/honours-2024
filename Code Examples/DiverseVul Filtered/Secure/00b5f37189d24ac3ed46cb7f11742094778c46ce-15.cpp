void kvm_lapic_expired_hv_timer(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;

	preempt_disable();
	/* If the preempt notifier has already run, it also called apic_timer_expired */
	if (!apic->lapic_timer.hv_timer_in_use)
		goto out;
	WARN_ON(kvm_vcpu_is_blocking(vcpu));
	apic_timer_expired(apic, false);
	cancel_hv_timer(apic);

	if (apic_lvtt_period(apic) && apic->lapic_timer.period) {
		advance_periodic_target_expiration(apic);
		restart_apic_timer(apic);
	}
out:
	preempt_enable();
}