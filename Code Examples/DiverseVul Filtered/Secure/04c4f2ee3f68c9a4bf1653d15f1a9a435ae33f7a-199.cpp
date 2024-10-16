static void vmx_msr_filter_changed(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 i;

	/*
	 * Set intercept permissions for all potentially passed through MSRs
	 * again. They will automatically get filtered through the MSR filter,
	 * so we are back in sync after this.
	 */
	for (i = 0; i < ARRAY_SIZE(vmx_possible_passthrough_msrs); i++) {
		u32 msr = vmx_possible_passthrough_msrs[i];
		bool read = test_bit(i, vmx->shadow_msr_intercept.read);
		bool write = test_bit(i, vmx->shadow_msr_intercept.write);

		vmx_set_intercept_for_msr(vcpu, msr, MSR_TYPE_R, read);
		vmx_set_intercept_for_msr(vcpu, msr, MSR_TYPE_W, write);
	}

	pt_update_intercept_for_msr(vcpu);
	vmx_update_msr_bitmap_x2apic(vcpu, vmx_msr_bitmap_mode(vcpu));
}