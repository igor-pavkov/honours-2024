int x86_emulate_instruction(struct kvm_vcpu *vcpu,
			    unsigned long cr2,
			    int emulation_type,
			    void *insn,
			    int insn_len)
{
	int r;
	struct x86_emulate_ctxt *ctxt = &vcpu->arch.emulate_ctxt;
	bool writeback = true;
	bool write_fault_to_spt = vcpu->arch.write_fault_to_shadow_pgtable;

	/*
	 * Clear write_fault_to_shadow_pgtable here to ensure it is
	 * never reused.
	 */
	vcpu->arch.write_fault_to_shadow_pgtable = false;
	kvm_clear_exception_queue(vcpu);

	if (!(emulation_type & EMULTYPE_NO_DECODE)) {
		init_emulate_ctxt(vcpu);
		ctxt->interruptibility = 0;
		ctxt->have_exception = false;
		ctxt->perm_ok = false;

		ctxt->only_vendor_specific_insn
			= emulation_type & EMULTYPE_TRAP_UD;

		r = x86_decode_insn(ctxt, insn, insn_len);

		trace_kvm_emulate_insn_start(vcpu);
		++vcpu->stat.insn_emulation;
		if (r != EMULATION_OK)  {
			if (emulation_type & EMULTYPE_TRAP_UD)
				return EMULATE_FAIL;
			if (reexecute_instruction(vcpu, cr2,
						  write_fault_to_spt))
				return EMULATE_DONE;
			if (emulation_type & EMULTYPE_SKIP)
				return EMULATE_FAIL;
			return handle_emulation_failure(vcpu);
		}
	}

	if (emulation_type & EMULTYPE_SKIP) {
		kvm_rip_write(vcpu, ctxt->_eip);
		return EMULATE_DONE;
	}

	if (retry_instruction(ctxt, cr2, emulation_type))
		return EMULATE_DONE;

	/* this is needed for vmware backdoor interface to work since it
	   changes registers values  during IO operation */
	if (vcpu->arch.emulate_regs_need_sync_from_vcpu) {
		vcpu->arch.emulate_regs_need_sync_from_vcpu = false;
		emulator_invalidate_register_cache(ctxt);
	}

restart:
	r = x86_emulate_insn(ctxt);

	if (r == EMULATION_INTERCEPTED)
		return EMULATE_DONE;

	if (r == EMULATION_FAILED) {
		if (reexecute_instruction(vcpu, cr2, write_fault_to_spt))
			return EMULATE_DONE;

		return handle_emulation_failure(vcpu);
	}

	if (ctxt->have_exception) {
		inject_emulated_exception(vcpu);
		r = EMULATE_DONE;
	} else if (vcpu->arch.pio.count) {
		if (!vcpu->arch.pio.in)
			vcpu->arch.pio.count = 0;
		else {
			writeback = false;
			vcpu->arch.complete_userspace_io = complete_emulated_pio;
		}
		r = EMULATE_DO_MMIO;
	} else if (vcpu->mmio_needed) {
		if (!vcpu->mmio_is_write)
			writeback = false;
		r = EMULATE_DO_MMIO;
		vcpu->arch.complete_userspace_io = complete_emulated_mmio;
	} else if (r == EMULATION_RESTART)
		goto restart;
	else
		r = EMULATE_DONE;

	if (writeback) {
		toggle_interruptibility(vcpu, ctxt->interruptibility);
		kvm_set_rflags(vcpu, ctxt->eflags);
		kvm_make_request(KVM_REQ_EVENT, vcpu);
		vcpu->arch.emulate_regs_need_sync_to_vcpu = false;
		kvm_rip_write(vcpu, ctxt->eip);
	} else
		vcpu->arch.emulate_regs_need_sync_to_vcpu = true;

	return r;
}