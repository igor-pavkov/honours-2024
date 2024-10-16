static __always_inline void vmx_disable_intercept_for_msr(struct kvm_vcpu *vcpu,
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
	 * for resync when the MSR filters change.
	*/
	if (is_valid_passthrough_msr(msr)) {
		int idx = possible_passthrough_msr_slot(msr);

		if (idx != -ENOENT) {
			if (type & MSR_TYPE_R)
				clear_bit(idx, vmx->shadow_msr_intercept.read);
			if (type & MSR_TYPE_W)
				clear_bit(idx, vmx->shadow_msr_intercept.write);
		}
	}

	if ((type & MSR_TYPE_R) &&
	    !kvm_msr_allowed(vcpu, msr, KVM_MSR_FILTER_READ)) {
		vmx_set_msr_bitmap_read(msr_bitmap, msr);
		type &= ~MSR_TYPE_R;
	}

	if ((type & MSR_TYPE_W) &&
	    !kvm_msr_allowed(vcpu, msr, KVM_MSR_FILTER_WRITE)) {
		vmx_set_msr_bitmap_write(msr_bitmap, msr);
		type &= ~MSR_TYPE_W;
	}

	if (type & MSR_TYPE_R)
		vmx_clear_msr_bitmap_read(msr_bitmap, msr);

	if (type & MSR_TYPE_W)
		vmx_clear_msr_bitmap_write(msr_bitmap, msr);
}