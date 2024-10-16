static int nested_vmx_check_vmentry_prereqs(struct kvm_vcpu *vcpu,
					    struct vmcs12 *vmcs12)
{
	if (nested_check_vm_execution_controls(vcpu, vmcs12) ||
	    nested_check_vm_exit_controls(vcpu, vmcs12) ||
	    nested_check_vm_entry_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_check_host_control_regs(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_HOST_STATE_FIELD;

	if (nested_check_guest_non_reg_state(vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	return 0;
}