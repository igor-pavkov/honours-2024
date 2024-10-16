static int nested_check_vm_exit_controls(struct kvm_vcpu *vcpu,
                                         struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx_control_verify(vmcs12->vm_exit_controls,
				vmx->nested.msrs.exit_ctls_low,
				vmx->nested.msrs.exit_ctls_high) ||
	    nested_vmx_check_exit_msr_switch_controls(vcpu, vmcs12))
		return -EINVAL;

	return 0;
}