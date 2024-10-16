static int nested_vmx_check_exit_msr_switch_controls(struct kvm_vcpu *vcpu,
						     struct vmcs12 *vmcs12)
{
	if (nested_vmx_check_msr_switch(vcpu, vmcs12->vm_exit_msr_load_count,
					vmcs12->vm_exit_msr_load_addr) ||
	    nested_vmx_check_msr_switch(vcpu, vmcs12->vm_exit_msr_store_count,
					vmcs12->vm_exit_msr_store_addr))
		return -EINVAL;

	return 0;
}