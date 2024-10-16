static int nested_vmx_run(struct kvm_vcpu *vcpu, bool launch)
{
	struct vmcs12 *vmcs12;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 interrupt_shadow = vmx_get_interrupt_shadow(vcpu);
	int ret;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (!nested_vmx_handle_enlightened_vmptrld(vcpu, true))
		return 1;

	if (!vmx->nested.hv_evmcs && vmx->nested.current_vmptr == -1ull)
		return nested_vmx_failInvalid(vcpu);

	vmcs12 = get_vmcs12(vcpu);

	/*
	 * Can't VMLAUNCH or VMRESUME a shadow VMCS. Despite the fact
	 * that there *is* a valid VMCS pointer, RFLAGS.CF is set
	 * rather than RFLAGS.ZF, and no error number is stored to the
	 * VM-instruction error field.
	 */
	if (vmcs12->hdr.shadow_vmcs)
		return nested_vmx_failInvalid(vcpu);

	if (vmx->nested.hv_evmcs) {
		copy_enlightened_to_vmcs12(vmx);
		/* Enlightened VMCS doesn't have launch state */
		vmcs12->launch_state = !launch;
	} else if (enable_shadow_vmcs) {
		copy_shadow_to_vmcs12(vmx);
	}

	/*
	 * The nested entry process starts with enforcing various prerequisites
	 * on vmcs12 as required by the Intel SDM, and act appropriately when
	 * they fail: As the SDM explains, some conditions should cause the
	 * instruction to fail, while others will cause the instruction to seem
	 * to succeed, but return an EXIT_REASON_INVALID_STATE.
	 * To speed up the normal (success) code path, we should avoid checking
	 * for misconfigurations which will anyway be caught by the processor
	 * when using the merged vmcs02.
	 */
	if (interrupt_shadow & KVM_X86_SHADOW_INT_MOV_SS)
		return nested_vmx_failValid(vcpu,
			VMXERR_ENTRY_EVENTS_BLOCKED_BY_MOV_SS);

	if (vmcs12->launch_state == launch)
		return nested_vmx_failValid(vcpu,
			launch ? VMXERR_VMLAUNCH_NONCLEAR_VMCS
			       : VMXERR_VMRESUME_NONLAUNCHED_VMCS);

	ret = nested_vmx_check_vmentry_prereqs(vcpu, vmcs12);
	if (ret)
		return nested_vmx_failValid(vcpu, ret);

	/*
	 * We're finally done with prerequisite checking, and can start with
	 * the nested entry.
	 */
	vmx->nested.nested_run_pending = 1;
	ret = nested_vmx_enter_non_root_mode(vcpu, true);
	vmx->nested.nested_run_pending = !ret;
	if (ret > 0)
		return 1;
	else if (ret)
		return nested_vmx_failValid(vcpu,
			VMXERR_ENTRY_INVALID_CONTROL_FIELD);

	/* Hide L1D cache contents from the nested guest.  */
	vmx->vcpu.arch.l1tf_flush_l1d = true;

	/*
	 * Must happen outside of nested_vmx_enter_non_root_mode() as it will
	 * also be used as part of restoring nVMX state for
	 * snapshot restore (migration).
	 *
	 * In this flow, it is assumed that vmcs12 cache was
	 * trasferred as part of captured nVMX state and should
	 * therefore not be read from guest memory (which may not
	 * exist on destination host yet).
	 */
	nested_cache_shadow_vmcs12(vcpu, vmcs12);

	/*
	 * If we're entering a halted L2 vcpu and the L2 vcpu won't be
	 * awakened by event injection or by an NMI-window VM-exit or
	 * by an interrupt-window VM-exit, halt the vcpu.
	 */
	if ((vmcs12->guest_activity_state == GUEST_ACTIVITY_HLT) &&
	    !(vmcs12->vm_entry_intr_info_field & INTR_INFO_VALID_MASK) &&
	    !(vmcs12->cpu_based_vm_exec_control & CPU_BASED_VIRTUAL_NMI_PENDING) &&
	    !((vmcs12->cpu_based_vm_exec_control & CPU_BASED_VIRTUAL_INTR_PENDING) &&
	      (vmcs12->guest_rflags & X86_EFLAGS_IF))) {
		vmx->nested.nested_run_pending = 0;
		return kvm_vcpu_halt(vcpu);
	}
	return 1;
}