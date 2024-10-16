static int nested_check_vm_execution_controls(struct kvm_vcpu *vcpu,
                                              struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx_control_verify(vmcs12->pin_based_vm_exec_control,
				vmx->nested.msrs.pinbased_ctls_low,
				vmx->nested.msrs.pinbased_ctls_high) ||
	    !vmx_control_verify(vmcs12->cpu_based_vm_exec_control,
				vmx->nested.msrs.procbased_ctls_low,
				vmx->nested.msrs.procbased_ctls_high))
		return -EINVAL;

	if (nested_cpu_has(vmcs12, CPU_BASED_ACTIVATE_SECONDARY_CONTROLS) &&
	    !vmx_control_verify(vmcs12->secondary_vm_exec_control,
				 vmx->nested.msrs.secondary_ctls_low,
				 vmx->nested.msrs.secondary_ctls_high))
		return -EINVAL;

	if (vmcs12->cr3_target_count > nested_cpu_vmx_misc_cr3_count(vcpu) ||
	    nested_vmx_check_io_bitmap_controls(vcpu, vmcs12) ||
	    nested_vmx_check_msr_bitmap_controls(vcpu, vmcs12) ||
	    nested_vmx_check_tpr_shadow_controls(vcpu, vmcs12) ||
	    nested_vmx_check_apic_access_controls(vcpu, vmcs12) ||
	    nested_vmx_check_apicv_controls(vcpu, vmcs12) ||
	    nested_vmx_check_nmi_controls(vmcs12) ||
	    nested_vmx_check_pml_controls(vcpu, vmcs12) ||
	    nested_vmx_check_unrestricted_guest_controls(vcpu, vmcs12) ||
	    nested_vmx_check_mode_based_ept_exec_controls(vcpu, vmcs12) ||
	    nested_vmx_check_shadow_vmcs_controls(vcpu, vmcs12) ||
	    (nested_cpu_has_vpid(vmcs12) && !vmcs12->virtual_processor_id))
		return -EINVAL;

	if (!nested_cpu_has_preemption_timer(vmcs12) &&
	    nested_cpu_has_save_preemption_timer(vmcs12))
		return -EINVAL;

	if (nested_cpu_has_ept(vmcs12) &&
	    !valid_ept_address(vcpu, vmcs12->ept_pointer))
		return -EINVAL;

	if (nested_cpu_has_vmfunc(vmcs12)) {
		if (vmcs12->vm_function_control &
		    ~vmx->nested.msrs.vmfunc_controls)
			return -EINVAL;

		if (nested_cpu_has_eptp_switching(vmcs12)) {
			if (!nested_cpu_has_ept(vmcs12) ||
			    !page_address_valid(vcpu, vmcs12->eptp_list_address))
				return -EINVAL;
		}
	}

	return 0;
}