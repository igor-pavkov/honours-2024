int kvm_x2apic_icr_write(struct kvm_lapic *apic, u64 data)
{
	data &= ~APIC_ICR_BUSY;

	kvm_apic_send_ipi(apic, (u32)data, (u32)(data >> 32));
	kvm_lapic_set_reg64(apic, APIC_ICR, data);
	trace_kvm_apic_write(APIC_ICR, data);
	return 0;
}