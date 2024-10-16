static int copy_enlightened_to_vmcs12(struct vcpu_vmx *vmx)
{
	struct vmcs12 *vmcs12 = vmx->nested.cached_vmcs12;
	struct hv_enlightened_vmcs *evmcs = vmx->nested.hv_evmcs;

	/* HV_VMX_ENLIGHTENED_CLEAN_FIELD_NONE */
	vmcs12->tpr_threshold = evmcs->tpr_threshold;
	vmcs12->guest_rip = evmcs->guest_rip;

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_GUEST_BASIC))) {
		vmcs12->guest_rsp = evmcs->guest_rsp;
		vmcs12->guest_rflags = evmcs->guest_rflags;
		vmcs12->guest_interruptibility_info =
			evmcs->guest_interruptibility_info;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_PROC))) {
		vmcs12->cpu_based_vm_exec_control =
			evmcs->cpu_based_vm_exec_control;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_PROC))) {
		vmcs12->exception_bitmap = evmcs->exception_bitmap;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_ENTRY))) {
		vmcs12->vm_entry_controls = evmcs->vm_entry_controls;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_EVENT))) {
		vmcs12->vm_entry_intr_info_field =
			evmcs->vm_entry_intr_info_field;
		vmcs12->vm_entry_exception_error_code =
			evmcs->vm_entry_exception_error_code;
		vmcs12->vm_entry_instruction_len =
			evmcs->vm_entry_instruction_len;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_HOST_GRP1))) {
		vmcs12->host_ia32_pat = evmcs->host_ia32_pat;
		vmcs12->host_ia32_efer = evmcs->host_ia32_efer;
		vmcs12->host_cr0 = evmcs->host_cr0;
		vmcs12->host_cr3 = evmcs->host_cr3;
		vmcs12->host_cr4 = evmcs->host_cr4;
		vmcs12->host_ia32_sysenter_esp = evmcs->host_ia32_sysenter_esp;
		vmcs12->host_ia32_sysenter_eip = evmcs->host_ia32_sysenter_eip;
		vmcs12->host_rip = evmcs->host_rip;
		vmcs12->host_ia32_sysenter_cs = evmcs->host_ia32_sysenter_cs;
		vmcs12->host_es_selector = evmcs->host_es_selector;
		vmcs12->host_cs_selector = evmcs->host_cs_selector;
		vmcs12->host_ss_selector = evmcs->host_ss_selector;
		vmcs12->host_ds_selector = evmcs->host_ds_selector;
		vmcs12->host_fs_selector = evmcs->host_fs_selector;
		vmcs12->host_gs_selector = evmcs->host_gs_selector;
		vmcs12->host_tr_selector = evmcs->host_tr_selector;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_HOST_GRP1))) {
		vmcs12->pin_based_vm_exec_control =
			evmcs->pin_based_vm_exec_control;
		vmcs12->vm_exit_controls = evmcs->vm_exit_controls;
		vmcs12->secondary_vm_exec_control =
			evmcs->secondary_vm_exec_control;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_IO_BITMAP))) {
		vmcs12->io_bitmap_a = evmcs->io_bitmap_a;
		vmcs12->io_bitmap_b = evmcs->io_bitmap_b;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_MSR_BITMAP))) {
		vmcs12->msr_bitmap = evmcs->msr_bitmap;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_GUEST_GRP2))) {
		vmcs12->guest_es_base = evmcs->guest_es_base;
		vmcs12->guest_cs_base = evmcs->guest_cs_base;
		vmcs12->guest_ss_base = evmcs->guest_ss_base;
		vmcs12->guest_ds_base = evmcs->guest_ds_base;
		vmcs12->guest_fs_base = evmcs->guest_fs_base;
		vmcs12->guest_gs_base = evmcs->guest_gs_base;
		vmcs12->guest_ldtr_base = evmcs->guest_ldtr_base;
		vmcs12->guest_tr_base = evmcs->guest_tr_base;
		vmcs12->guest_gdtr_base = evmcs->guest_gdtr_base;
		vmcs12->guest_idtr_base = evmcs->guest_idtr_base;
		vmcs12->guest_es_limit = evmcs->guest_es_limit;
		vmcs12->guest_cs_limit = evmcs->guest_cs_limit;
		vmcs12->guest_ss_limit = evmcs->guest_ss_limit;
		vmcs12->guest_ds_limit = evmcs->guest_ds_limit;
		vmcs12->guest_fs_limit = evmcs->guest_fs_limit;
		vmcs12->guest_gs_limit = evmcs->guest_gs_limit;
		vmcs12->guest_ldtr_limit = evmcs->guest_ldtr_limit;
		vmcs12->guest_tr_limit = evmcs->guest_tr_limit;
		vmcs12->guest_gdtr_limit = evmcs->guest_gdtr_limit;
		vmcs12->guest_idtr_limit = evmcs->guest_idtr_limit;
		vmcs12->guest_es_ar_bytes = evmcs->guest_es_ar_bytes;
		vmcs12->guest_cs_ar_bytes = evmcs->guest_cs_ar_bytes;
		vmcs12->guest_ss_ar_bytes = evmcs->guest_ss_ar_bytes;
		vmcs12->guest_ds_ar_bytes = evmcs->guest_ds_ar_bytes;
		vmcs12->guest_fs_ar_bytes = evmcs->guest_fs_ar_bytes;
		vmcs12->guest_gs_ar_bytes = evmcs->guest_gs_ar_bytes;
		vmcs12->guest_ldtr_ar_bytes = evmcs->guest_ldtr_ar_bytes;
		vmcs12->guest_tr_ar_bytes = evmcs->guest_tr_ar_bytes;
		vmcs12->guest_es_selector = evmcs->guest_es_selector;
		vmcs12->guest_cs_selector = evmcs->guest_cs_selector;
		vmcs12->guest_ss_selector = evmcs->guest_ss_selector;
		vmcs12->guest_ds_selector = evmcs->guest_ds_selector;
		vmcs12->guest_fs_selector = evmcs->guest_fs_selector;
		vmcs12->guest_gs_selector = evmcs->guest_gs_selector;
		vmcs12->guest_ldtr_selector = evmcs->guest_ldtr_selector;
		vmcs12->guest_tr_selector = evmcs->guest_tr_selector;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_GRP2))) {
		vmcs12->tsc_offset = evmcs->tsc_offset;
		vmcs12->virtual_apic_page_addr = evmcs->virtual_apic_page_addr;
		vmcs12->xss_exit_bitmap = evmcs->xss_exit_bitmap;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CRDR))) {
		vmcs12->cr0_guest_host_mask = evmcs->cr0_guest_host_mask;
		vmcs12->cr4_guest_host_mask = evmcs->cr4_guest_host_mask;
		vmcs12->cr0_read_shadow = evmcs->cr0_read_shadow;
		vmcs12->cr4_read_shadow = evmcs->cr4_read_shadow;
		vmcs12->guest_cr0 = evmcs->guest_cr0;
		vmcs12->guest_cr3 = evmcs->guest_cr3;
		vmcs12->guest_cr4 = evmcs->guest_cr4;
		vmcs12->guest_dr7 = evmcs->guest_dr7;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_HOST_POINTER))) {
		vmcs12->host_fs_base = evmcs->host_fs_base;
		vmcs12->host_gs_base = evmcs->host_gs_base;
		vmcs12->host_tr_base = evmcs->host_tr_base;
		vmcs12->host_gdtr_base = evmcs->host_gdtr_base;
		vmcs12->host_idtr_base = evmcs->host_idtr_base;
		vmcs12->host_rsp = evmcs->host_rsp;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_CONTROL_XLAT))) {
		vmcs12->ept_pointer = evmcs->ept_pointer;
		vmcs12->virtual_processor_id = evmcs->virtual_processor_id;
	}

	if (unlikely(!(evmcs->hv_clean_fields &
		       HV_VMX_ENLIGHTENED_CLEAN_FIELD_GUEST_GRP1))) {
		vmcs12->vmcs_link_pointer = evmcs->vmcs_link_pointer;
		vmcs12->guest_ia32_debugctl = evmcs->guest_ia32_debugctl;
		vmcs12->guest_ia32_pat = evmcs->guest_ia32_pat;
		vmcs12->guest_ia32_efer = evmcs->guest_ia32_efer;
		vmcs12->guest_pdptr0 = evmcs->guest_pdptr0;
		vmcs12->guest_pdptr1 = evmcs->guest_pdptr1;
		vmcs12->guest_pdptr2 = evmcs->guest_pdptr2;
		vmcs12->guest_pdptr3 = evmcs->guest_pdptr3;
		vmcs12->guest_pending_dbg_exceptions =
			evmcs->guest_pending_dbg_exceptions;
		vmcs12->guest_sysenter_esp = evmcs->guest_sysenter_esp;
		vmcs12->guest_sysenter_eip = evmcs->guest_sysenter_eip;
		vmcs12->guest_bndcfgs = evmcs->guest_bndcfgs;
		vmcs12->guest_activity_state = evmcs->guest_activity_state;
		vmcs12->guest_sysenter_cs = evmcs->guest_sysenter_cs;
	}

	/*
	 * Not used?
	 * vmcs12->vm_exit_msr_store_addr = evmcs->vm_exit_msr_store_addr;
	 * vmcs12->vm_exit_msr_load_addr = evmcs->vm_exit_msr_load_addr;
	 * vmcs12->vm_entry_msr_load_addr = evmcs->vm_entry_msr_load_addr;
	 * vmcs12->cr3_target_value0 = evmcs->cr3_target_value0;
	 * vmcs12->cr3_target_value1 = evmcs->cr3_target_value1;
	 * vmcs12->cr3_target_value2 = evmcs->cr3_target_value2;
	 * vmcs12->cr3_target_value3 = evmcs->cr3_target_value3;
	 * vmcs12->page_fault_error_code_mask =
	 *		evmcs->page_fault_error_code_mask;
	 * vmcs12->page_fault_error_code_match =
	 *		evmcs->page_fault_error_code_match;
	 * vmcs12->cr3_target_count = evmcs->cr3_target_count;
	 * vmcs12->vm_exit_msr_store_count = evmcs->vm_exit_msr_store_count;
	 * vmcs12->vm_exit_msr_load_count = evmcs->vm_exit_msr_load_count;
	 * vmcs12->vm_entry_msr_load_count = evmcs->vm_entry_msr_load_count;
	 */

	/*
	 * Read only fields:
	 * vmcs12->guest_physical_address = evmcs->guest_physical_address;
	 * vmcs12->vm_instruction_error = evmcs->vm_instruction_error;
	 * vmcs12->vm_exit_reason = evmcs->vm_exit_reason;
	 * vmcs12->vm_exit_intr_info = evmcs->vm_exit_intr_info;
	 * vmcs12->vm_exit_intr_error_code = evmcs->vm_exit_intr_error_code;
	 * vmcs12->idt_vectoring_info_field = evmcs->idt_vectoring_info_field;
	 * vmcs12->idt_vectoring_error_code = evmcs->idt_vectoring_error_code;
	 * vmcs12->vm_exit_instruction_len = evmcs->vm_exit_instruction_len;
	 * vmcs12->vmx_instruction_info = evmcs->vmx_instruction_info;
	 * vmcs12->exit_qualification = evmcs->exit_qualification;
	 * vmcs12->guest_linear_address = evmcs->guest_linear_address;
	 *
	 * Not present in struct vmcs12:
	 * vmcs12->exit_io_instruction_ecx = evmcs->exit_io_instruction_ecx;
	 * vmcs12->exit_io_instruction_esi = evmcs->exit_io_instruction_esi;
	 * vmcs12->exit_io_instruction_edi = evmcs->exit_io_instruction_edi;
	 * vmcs12->exit_io_instruction_eip = evmcs->exit_io_instruction_eip;
	 */

	return 0;
}