static int nested_vmx_check_mode_based_ept_exec_controls(struct kvm_vcpu *vcpu,
							 struct vmcs12 *vmcs12)
{
	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_MODE_BASED_EPT_EXEC) &&
	    !nested_cpu_has_ept(vmcs12))
		return -EINVAL;
	return 0;
}