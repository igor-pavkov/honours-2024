void vmx_update_cpu_dirty_logging(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (is_guest_mode(vcpu)) {
		vmx->nested.update_vmcs01_cpu_dirty_logging = true;
		return;
	}

	/*
	 * Note, cpu_dirty_logging_count can be changed concurrent with this
	 * code, but in that case another update request will be made and so
	 * the guest will never run with a stale PML value.
	 */
	if (vcpu->kvm->arch.cpu_dirty_logging_count)
		secondary_exec_controls_setbit(vmx, SECONDARY_EXEC_ENABLE_PML);
	else
		secondary_exec_controls_clearbit(vmx, SECONDARY_EXEC_ENABLE_PML);
}