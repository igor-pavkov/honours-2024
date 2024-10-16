static void prepare_vmcs02_early(struct vcpu_vmx *vmx, struct vmcs12 *vmcs12)
{
	u32 exec_control, vmcs12_exec_ctrl;
	u64 guest_efer = nested_vmx_calc_efer(vmx, vmcs12);

	if (vmx->nested.dirty_vmcs12 || vmx->nested.hv_evmcs)
		prepare_vmcs02_early_full(vmx, vmcs12);

	/*
	 * PIN CONTROLS
	 */
	exec_control = vmcs12->pin_based_vm_exec_control;

	/* Preemption timer setting is computed directly in vmx_vcpu_run.  */
	exec_control |= vmcs_config.pin_based_exec_ctrl;
	exec_control &= ~PIN_BASED_VMX_PREEMPTION_TIMER;
	vmx->loaded_vmcs->hv_timer_armed = false;

	/* Posted interrupts setting is only taken from vmcs12.  */
	if (nested_cpu_has_posted_intr(vmcs12)) {
		vmx->nested.posted_intr_nv = vmcs12->posted_intr_nv;
		vmx->nested.pi_pending = false;
	} else {
		exec_control &= ~PIN_BASED_POSTED_INTR;
	}
	vmcs_write32(PIN_BASED_VM_EXEC_CONTROL, exec_control);

	/*
	 * EXEC CONTROLS
	 */
	exec_control = vmx_exec_control(vmx); /* L0's desires */
	exec_control &= ~CPU_BASED_VIRTUAL_INTR_PENDING;
	exec_control &= ~CPU_BASED_VIRTUAL_NMI_PENDING;
	exec_control &= ~CPU_BASED_TPR_SHADOW;
	exec_control |= vmcs12->cpu_based_vm_exec_control;

	/*
	 * Write an illegal value to VIRTUAL_APIC_PAGE_ADDR. Later, if
	 * nested_get_vmcs12_pages can't fix it up, the illegal value
	 * will result in a VM entry failure.
	 */
	if (exec_control & CPU_BASED_TPR_SHADOW) {
		vmcs_write64(VIRTUAL_APIC_PAGE_ADDR, -1ull);
		vmcs_write32(TPR_THRESHOLD, vmcs12->tpr_threshold);
	} else {
#ifdef CONFIG_X86_64
		exec_control |= CPU_BASED_CR8_LOAD_EXITING |
				CPU_BASED_CR8_STORE_EXITING;
#endif
	}

	/*
	 * A vmexit (to either L1 hypervisor or L0 userspace) is always needed
	 * for I/O port accesses.
	 */
	exec_control &= ~CPU_BASED_USE_IO_BITMAPS;
	exec_control |= CPU_BASED_UNCOND_IO_EXITING;
	vmcs_write32(CPU_BASED_VM_EXEC_CONTROL, exec_control);

	/*
	 * SECONDARY EXEC CONTROLS
	 */
	if (cpu_has_secondary_exec_ctrls()) {
		exec_control = vmx->secondary_exec_control;

		/* Take the following fields only from vmcs12 */
		exec_control &= ~(SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
				  SECONDARY_EXEC_ENABLE_INVPCID |
				  SECONDARY_EXEC_RDTSCP |
				  SECONDARY_EXEC_XSAVES |
				  SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY |
				  SECONDARY_EXEC_APIC_REGISTER_VIRT |
				  SECONDARY_EXEC_ENABLE_VMFUNC);
		if (nested_cpu_has(vmcs12,
				   CPU_BASED_ACTIVATE_SECONDARY_CONTROLS)) {
			vmcs12_exec_ctrl = vmcs12->secondary_vm_exec_control &
				~SECONDARY_EXEC_ENABLE_PML;
			exec_control |= vmcs12_exec_ctrl;
		}

		/* VMCS shadowing for L2 is emulated for now */
		exec_control &= ~SECONDARY_EXEC_SHADOW_VMCS;

		if (exec_control & SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY)
			vmcs_write16(GUEST_INTR_STATUS,
				vmcs12->guest_intr_status);

		/*
		 * Write an illegal value to APIC_ACCESS_ADDR. Later,
		 * nested_get_vmcs12_pages will either fix it up or
		 * remove the VM execution control.
		 */
		if (exec_control & SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)
			vmcs_write64(APIC_ACCESS_ADDR, -1ull);

		if (exec_control & SECONDARY_EXEC_ENCLS_EXITING)
			vmcs_write64(ENCLS_EXITING_BITMAP, -1ull);

		vmcs_write32(SECONDARY_VM_EXEC_CONTROL, exec_control);
	}

	/*
	 * ENTRY CONTROLS
	 *
	 * vmcs12's VM_{ENTRY,EXIT}_LOAD_IA32_EFER and VM_ENTRY_IA32E_MODE
	 * are emulated by vmx_set_efer() in prepare_vmcs02(), but speculate
	 * on the related bits (if supported by the CPU) in the hope that
	 * we can avoid VMWrites during vmx_set_efer().
	 */
	exec_control = (vmcs12->vm_entry_controls | vmx_vmentry_ctrl()) &
			~VM_ENTRY_IA32E_MODE & ~VM_ENTRY_LOAD_IA32_EFER;
	if (cpu_has_load_ia32_efer()) {
		if (guest_efer & EFER_LMA)
			exec_control |= VM_ENTRY_IA32E_MODE;
		if (guest_efer != host_efer)
			exec_control |= VM_ENTRY_LOAD_IA32_EFER;
	}
	vm_entry_controls_init(vmx, exec_control);

	/*
	 * EXIT CONTROLS
	 *
	 * L2->L1 exit controls are emulated - the hardware exit is to L0 so
	 * we should use its exit controls. Note that VM_EXIT_LOAD_IA32_EFER
	 * bits may be modified by vmx_set_efer() in prepare_vmcs02().
	 */
	exec_control = vmx_vmexit_ctrl();
	if (cpu_has_load_ia32_efer() && guest_efer != host_efer)
		exec_control |= VM_EXIT_LOAD_IA32_EFER;
	vm_exit_controls_init(vmx, exec_control);

	/*
	 * Conceptually we want to copy the PML address and index from
	 * vmcs01 here, and then back to vmcs01 on nested vmexit. But,
	 * since we always flush the log on each vmexit and never change
	 * the PML address (once set), this happens to be equivalent to
	 * simply resetting the index in vmcs02.
	 */
	if (enable_pml)
		vmcs_write16(GUEST_PML_INDEX, PML_ENTITY_NUM - 1);

	/*
	 * Interrupt/Exception Fields
	 */
	if (vmx->nested.nested_run_pending) {
		vmcs_write32(VM_ENTRY_INTR_INFO_FIELD,
			     vmcs12->vm_entry_intr_info_field);
		vmcs_write32(VM_ENTRY_EXCEPTION_ERROR_CODE,
			     vmcs12->vm_entry_exception_error_code);
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN,
			     vmcs12->vm_entry_instruction_len);
		vmcs_write32(GUEST_INTERRUPTIBILITY_INFO,
			     vmcs12->guest_interruptibility_info);
		vmx->loaded_vmcs->nmi_known_unmasked =
			!(vmcs12->guest_interruptibility_info & GUEST_INTR_STATE_NMI);
	} else {
		vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, 0);
	}
}