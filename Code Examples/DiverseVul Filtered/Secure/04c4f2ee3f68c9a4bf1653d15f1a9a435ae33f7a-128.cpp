static void vmx_update_msr_bitmap_x2apic(struct kvm_vcpu *vcpu, u8 mode)
{
	if (!cpu_has_vmx_msr_bitmap())
		return;

	vmx_reset_x2apic_msrs(vcpu, mode);

	/*
	 * TPR reads and writes can be virtualized even if virtual interrupt
	 * delivery is not in use.
	 */
	vmx_set_intercept_for_msr(vcpu, X2APIC_MSR(APIC_TASKPRI), MSR_TYPE_RW,
				  !(mode & MSR_BITMAP_MODE_X2APIC));

	if (mode & MSR_BITMAP_MODE_X2APIC_APICV) {
		vmx_enable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_TMCCT), MSR_TYPE_RW);
		vmx_disable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_EOI), MSR_TYPE_W);
		vmx_disable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_SELF_IPI), MSR_TYPE_W);
	}
}