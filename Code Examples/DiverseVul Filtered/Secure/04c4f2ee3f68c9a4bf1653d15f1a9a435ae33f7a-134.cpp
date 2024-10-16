static __always_inline void vmx_enable_intercept_for_msr(struct kvm_vcpu *vcpu,
							 u32 msr, int type)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long *msr_bitmap = vmx->vmcs01.msr_bitmap;

	if (!cpu_has_vmx_msr_bitmap())
		return;

	if (static_branch_unlikely(&enable_evmcs))
		evmcs_touch_msr_bitmap();

	/*
	 * Mark the desired intercept state in shadow bitmap, this is needed
	 * for resync when the MSR filter changes.
	*/
	if (is_valid_passthrough_msr(msr)) {
		int idx = possible_passthrough_msr_slot(msr);

		if (idx != -ENOENT) {
			if (type & MSR_TYPE_R)
				set_bit(idx, vmx->shadow_msr_intercept.read);
			if (type & MSR_TYPE_W)
				set_bit(idx, vmx->shadow_msr_intercept.write);
		}
	}

	if (type & MSR_TYPE_R)
		vmx_set_msr_bitmap_read(msr_bitmap, msr);

	if (type & MSR_TYPE_W)
		vmx_set_msr_bitmap_write(msr_bitmap, msr);
}