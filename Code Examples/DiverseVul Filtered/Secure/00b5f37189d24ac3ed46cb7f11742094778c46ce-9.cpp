static void apic_sync_pv_eoi_from_guest(struct kvm_vcpu *vcpu,
					struct kvm_lapic *apic)
{
	int vector;
	/*
	 * PV EOI state is derived from KVM_APIC_PV_EOI_PENDING in host
	 * and KVM_PV_EOI_ENABLED in guest memory as follows:
	 *
	 * KVM_APIC_PV_EOI_PENDING is unset:
	 * 	-> host disabled PV EOI.
	 * KVM_APIC_PV_EOI_PENDING is set, KVM_PV_EOI_ENABLED is set:
	 * 	-> host enabled PV EOI, guest did not execute EOI yet.
	 * KVM_APIC_PV_EOI_PENDING is set, KVM_PV_EOI_ENABLED is unset:
	 * 	-> host enabled PV EOI, guest executed EOI.
	 */
	BUG_ON(!pv_eoi_enabled(vcpu));

	if (pv_eoi_test_and_clr_pending(vcpu))
		return;
	vector = apic_set_eoi(apic);
	trace_kvm_pv_eoi(apic, vector);
}