static int kvm_lapic_reg_write(struct kvm_lapic *apic, u32 reg, u32 val)
{
	int ret = 0;

	trace_kvm_apic_write(reg, val);

	switch (reg) {
	case APIC_ID:		/* Local APIC ID */
		if (!apic_x2apic_mode(apic))
			kvm_apic_set_xapic_id(apic, val >> 24);
		else
			ret = 1;
		break;

	case APIC_TASKPRI:
		report_tpr_access(apic, true);
		apic_set_tpr(apic, val & 0xff);
		break;

	case APIC_EOI:
		apic_set_eoi(apic);
		break;

	case APIC_LDR:
		if (!apic_x2apic_mode(apic))
			kvm_apic_set_ldr(apic, val & APIC_LDR_MASK);
		else
			ret = 1;
		break;

	case APIC_DFR:
		if (!apic_x2apic_mode(apic))
			kvm_apic_set_dfr(apic, val | 0x0FFFFFFF);
		else
			ret = 1;
		break;

	case APIC_SPIV: {
		u32 mask = 0x3ff;
		if (kvm_lapic_get_reg(apic, APIC_LVR) & APIC_LVR_DIRECTED_EOI)
			mask |= APIC_SPIV_DIRECTED_EOI;
		apic_set_spiv(apic, val & mask);
		if (!(val & APIC_SPIV_APIC_ENABLED)) {
			int i;
			u32 lvt_val;

			for (i = 0; i < KVM_APIC_LVT_NUM; i++) {
				lvt_val = kvm_lapic_get_reg(apic,
						       APIC_LVTT + 0x10 * i);
				kvm_lapic_set_reg(apic, APIC_LVTT + 0x10 * i,
					     lvt_val | APIC_LVT_MASKED);
			}
			apic_update_lvtt(apic);
			atomic_set(&apic->lapic_timer.pending, 0);

		}
		break;
	}
	case APIC_ICR:
		WARN_ON_ONCE(apic_x2apic_mode(apic));

		/* No delay here, so we always clear the pending bit */
		val &= ~APIC_ICR_BUSY;
		kvm_apic_send_ipi(apic, val, kvm_lapic_get_reg(apic, APIC_ICR2));
		kvm_lapic_set_reg(apic, APIC_ICR, val);
		break;
	case APIC_ICR2:
		if (apic_x2apic_mode(apic))
			ret = 1;
		else
			kvm_lapic_set_reg(apic, APIC_ICR2, val & 0xff000000);
		break;

	case APIC_LVT0:
		apic_manage_nmi_watchdog(apic, val);
		fallthrough;
	case APIC_LVTTHMR:
	case APIC_LVTPC:
	case APIC_LVT1:
	case APIC_LVTERR: {
		/* TODO: Check vector */
		size_t size;
		u32 index;

		if (!kvm_apic_sw_enabled(apic))
			val |= APIC_LVT_MASKED;
		size = ARRAY_SIZE(apic_lvt_mask);
		index = array_index_nospec(
				(reg - APIC_LVTT) >> 4, size);
		val &= apic_lvt_mask[index];
		kvm_lapic_set_reg(apic, reg, val);
		break;
	}

	case APIC_LVTT:
		if (!kvm_apic_sw_enabled(apic))
			val |= APIC_LVT_MASKED;
		val &= (apic_lvt_mask[0] | apic->lapic_timer.timer_mode_mask);
		kvm_lapic_set_reg(apic, APIC_LVTT, val);
		apic_update_lvtt(apic);
		break;

	case APIC_TMICT:
		if (apic_lvtt_tscdeadline(apic))
			break;

		cancel_apic_timer(apic);
		kvm_lapic_set_reg(apic, APIC_TMICT, val);
		start_apic_timer(apic);
		break;

	case APIC_TDCR: {
		uint32_t old_divisor = apic->divide_count;

		kvm_lapic_set_reg(apic, APIC_TDCR, val & 0xb);
		update_divide_count(apic);
		if (apic->divide_count != old_divisor &&
				apic->lapic_timer.period) {
			hrtimer_cancel(&apic->lapic_timer.timer);
			update_target_expiration(apic, old_divisor);
			restart_apic_timer(apic);
		}
		break;
	}
	case APIC_ESR:
		if (apic_x2apic_mode(apic) && val != 0)
			ret = 1;
		break;

	case APIC_SELF_IPI:
		if (apic_x2apic_mode(apic))
			kvm_apic_send_ipi(apic, APIC_DEST_SELF | (val & APIC_VECTOR_MASK), 0);
		else
			ret = 1;
		break;
	default:
		ret = 1;
		break;
	}

	/*
	 * Recalculate APIC maps if necessary, e.g. if the software enable bit
	 * was toggled, the APIC ID changed, etc...   The maps are marked dirty
	 * on relevant changes, i.e. this is a nop for most writes.
	 */
	kvm_recalculate_apic_map(apic->vcpu->kvm);

	return ret;
}