static void vmx_update_emulated_instruction(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!is_guest_mode(vcpu))
		return;

	/*
	 * Per the SDM, MTF takes priority over debug-trap exceptions besides
	 * T-bit traps. As instruction emulation is completed (i.e. at the
	 * instruction boundary), any #DB exception pending delivery must be a
	 * debug-trap. Record the pending MTF state to be delivered in
	 * vmx_check_nested_events().
	 */
	if (nested_cpu_has_mtf(vmcs12) &&
	    (!vcpu->arch.exception.pending ||
	     vcpu->arch.exception.nr == DB_VECTOR))
		vmx->nested.mtf_pending = true;
	else
		vmx->nested.mtf_pending = false;
}