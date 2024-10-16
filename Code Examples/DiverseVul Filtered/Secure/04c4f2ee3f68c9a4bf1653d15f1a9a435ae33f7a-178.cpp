static u8 vmx_msr_bitmap_mode(struct kvm_vcpu *vcpu)
{
	u8 mode = 0;

	if (cpu_has_secondary_exec_ctrls() &&
	    (secondary_exec_controls_get(to_vmx(vcpu)) &
	     SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE)) {
		mode |= MSR_BITMAP_MODE_X2APIC;
		if (enable_apicv && kvm_vcpu_apicv_active(vcpu))
			mode |= MSR_BITMAP_MODE_X2APIC_APICV;
	}

	return mode;
}