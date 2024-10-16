int kvm_apic_accept_events(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	u8 sipi_vector;
	int r;
	unsigned long pe;

	if (!lapic_in_kernel(vcpu))
		return 0;

	/*
	 * Read pending events before calling the check_events
	 * callback.
	 */
	pe = smp_load_acquire(&apic->pending_events);
	if (!pe)
		return 0;

	if (is_guest_mode(vcpu)) {
		r = kvm_check_nested_events(vcpu);
		if (r < 0)
			return r == -EBUSY ? 0 : r;
		/*
		 * If an event has happened and caused a vmexit,
		 * we know INITs are latched and therefore
		 * we will not incorrectly deliver an APIC
		 * event instead of a vmexit.
		 */
	}

	/*
	 * INITs are latched while CPU is in specific states
	 * (SMM, VMX root mode, SVM with GIF=0).
	 * Because a CPU cannot be in these states immediately
	 * after it has processed an INIT signal (and thus in
	 * KVM_MP_STATE_INIT_RECEIVED state), just eat SIPIs
	 * and leave the INIT pending.
	 */
	if (kvm_vcpu_latch_init(vcpu)) {
		WARN_ON_ONCE(vcpu->arch.mp_state == KVM_MP_STATE_INIT_RECEIVED);
		if (test_bit(KVM_APIC_SIPI, &pe))
			clear_bit(KVM_APIC_SIPI, &apic->pending_events);
		return 0;
	}

	if (test_bit(KVM_APIC_INIT, &pe)) {
		clear_bit(KVM_APIC_INIT, &apic->pending_events);
		kvm_vcpu_reset(vcpu, true);
		if (kvm_vcpu_is_bsp(apic->vcpu))
			vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
		else
			vcpu->arch.mp_state = KVM_MP_STATE_INIT_RECEIVED;
	}
	if (test_bit(KVM_APIC_SIPI, &pe)) {
		clear_bit(KVM_APIC_SIPI, &apic->pending_events);
		if (vcpu->arch.mp_state == KVM_MP_STATE_INIT_RECEIVED) {
			/* evaluate pending_events before reading the vector */
			smp_rmb();
			sipi_vector = apic->sipi_vector;
			static_call(kvm_x86_vcpu_deliver_sipi_vector)(vcpu, sipi_vector);
			vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
		}
	}
	return 0;
}