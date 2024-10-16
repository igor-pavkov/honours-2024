int kvm_apic_set_state(struct kvm_vcpu *vcpu, struct kvm_lapic_state *s)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	int r;

	kvm_lapic_set_base(vcpu, vcpu->arch.apic_base);
	/* set SPIV separately to get count of SW disabled APICs right */
	apic_set_spiv(apic, *((u32 *)(s->regs + APIC_SPIV)));

	r = kvm_apic_state_fixup(vcpu, s, true);
	if (r) {
		kvm_recalculate_apic_map(vcpu->kvm);
		return r;
	}
	memcpy(vcpu->arch.apic->regs, s->regs, sizeof(*s));

	atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
	kvm_recalculate_apic_map(vcpu->kvm);
	kvm_apic_set_version(vcpu);

	apic_update_ppr(apic);
	cancel_apic_timer(apic);
	apic->lapic_timer.expired_tscdeadline = 0;
	apic_update_lvtt(apic);
	apic_manage_nmi_watchdog(apic, kvm_lapic_get_reg(apic, APIC_LVT0));
	update_divide_count(apic);
	__start_apic_timer(apic, APIC_TMCCT);
	kvm_lapic_set_reg(apic, APIC_TMCCT, 0);
	kvm_apic_update_apicv(vcpu);
	apic->highest_isr_cache = -1;
	if (vcpu->arch.apicv_active) {
		static_call_cond(kvm_x86_apicv_post_state_restore)(vcpu);
		static_call_cond(kvm_x86_hwapic_irr_update)(vcpu, apic_find_highest_irr(apic));
		static_call_cond(kvm_x86_hwapic_isr_update)(vcpu, apic_find_highest_isr(apic));
	}
	kvm_make_request(KVM_REQ_EVENT, vcpu);
	if (ioapic_in_kernel(vcpu->kvm))
		kvm_rtc_eoi_tracking_restore_one(vcpu);

	vcpu->arch.apic_arb_prio = 0;

	return 0;
}