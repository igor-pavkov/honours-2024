static int kvm_handle_exit(struct kvm_run *kvm_run, struct kvm_vcpu *vcpu)
{
	u32 exit_reason = vmcs_read32(VM_EXIT_REASON);
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 vectoring_info = vmx->idt_vectoring_info;

	KVMTRACE_3D(VMEXIT, vcpu, exit_reason, (u32)kvm_rip_read(vcpu),
		    (u32)((u64)kvm_rip_read(vcpu) >> 32), entryexit);

	/* If we need to emulate an MMIO from handle_invalid_guest_state
	 * we just return 0 */
	if (vmx->emulation_required && emulate_invalid_guest_state) {
		if (guest_state_valid(vcpu))
			vmx->emulation_required = 0;
		return vmx->invalid_state_emulation_result != EMULATE_DO_MMIO;
	}

	/* Access CR3 don't cause VMExit in paging mode, so we need
	 * to sync with guest real CR3. */
	if (vm_need_ept() && is_paging(vcpu)) {
		vcpu->arch.cr3 = vmcs_readl(GUEST_CR3);
		ept_load_pdptrs(vcpu);
	}

	if (unlikely(vmx->fail)) {
		kvm_run->exit_reason = KVM_EXIT_FAIL_ENTRY;
		kvm_run->fail_entry.hardware_entry_failure_reason
			= vmcs_read32(VM_INSTRUCTION_ERROR);
		return 0;
	}

	if ((vectoring_info & VECTORING_INFO_VALID_MASK) &&
			(exit_reason != EXIT_REASON_EXCEPTION_NMI &&
			exit_reason != EXIT_REASON_EPT_VIOLATION &&
			exit_reason != EXIT_REASON_TASK_SWITCH))
		printk(KERN_WARNING "%s: unexpected, valid vectoring info "
		       "(0x%x) and exit reason is 0x%x\n",
		       __func__, vectoring_info, exit_reason);

	if (unlikely(!cpu_has_virtual_nmis() && vmx->soft_vnmi_blocked)) {
		if (vcpu->arch.interrupt_window_open) {
			vmx->soft_vnmi_blocked = 0;
			vcpu->arch.nmi_window_open = 1;
		} else if (vmx->vnmi_blocked_time > 1000000000LL &&
			   vcpu->arch.nmi_pending) {
			/*
			 * This CPU don't support us in finding the end of an
			 * NMI-blocked window if the guest runs with IRQs
			 * disabled. So we pull the trigger after 1 s of
			 * futile waiting, but inform the user about this.
			 */
			printk(KERN_WARNING "%s: Breaking out of NMI-blocked "
			       "state on VCPU %d after 1 s timeout\n",
			       __func__, vcpu->vcpu_id);
			vmx->soft_vnmi_blocked = 0;
			vmx->vcpu.arch.nmi_window_open = 1;
		}
	}

	if (exit_reason < kvm_vmx_max_exit_handlers
	    && kvm_vmx_exit_handlers[exit_reason])
		return kvm_vmx_exit_handlers[exit_reason](vcpu, kvm_run);
	else {
		kvm_run->exit_reason = KVM_EXIT_UNKNOWN;
		kvm_run->hw.hardware_exit_reason = exit_reason;
	}
	return 0;
}