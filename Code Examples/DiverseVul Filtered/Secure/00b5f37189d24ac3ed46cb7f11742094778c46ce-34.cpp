static int kvm_apic_state_fixup(struct kvm_vcpu *vcpu,
		struct kvm_lapic_state *s, bool set)
{
	if (apic_x2apic_mode(vcpu->arch.apic)) {
		u32 *id = (u32 *)(s->regs + APIC_ID);
		u32 *ldr = (u32 *)(s->regs + APIC_LDR);
		u64 icr;

		if (vcpu->kvm->arch.x2apic_format) {
			if (*id != vcpu->vcpu_id)
				return -EINVAL;
		} else {
			if (set)
				*id >>= 24;
			else
				*id <<= 24;
		}

		/*
		 * In x2APIC mode, the LDR is fixed and based on the id.  And
		 * ICR is internally a single 64-bit register, but needs to be
		 * split to ICR+ICR2 in userspace for backwards compatibility.
		 */
		if (set) {
			*ldr = kvm_apic_calc_x2apic_ldr(*id);

			icr = __kvm_lapic_get_reg(s->regs, APIC_ICR) |
			      (u64)__kvm_lapic_get_reg(s->regs, APIC_ICR2) << 32;
			__kvm_lapic_set_reg64(s->regs, APIC_ICR, icr);
		} else {
			icr = __kvm_lapic_get_reg64(s->regs, APIC_ICR);
			__kvm_lapic_set_reg(s->regs, APIC_ICR2, icr >> 32);
		}
	}

	return 0;
}