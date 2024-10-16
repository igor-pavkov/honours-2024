static int skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	unsigned long rip, orig_rip;

	/*
	 * Using VMCS.VM_EXIT_INSTRUCTION_LEN on EPT misconfig depends on
	 * undefined behavior: Intel's SDM doesn't mandate the VMCS field be
	 * set when EPT misconfig occurs.  In practice, real hardware updates
	 * VM_EXIT_INSTRUCTION_LEN on EPT misconfig, but other hypervisors
	 * (namely Hyper-V) don't set it due to it being undefined behavior,
	 * i.e. we end up advancing IP with some random value.
	 */
	if (!static_cpu_has(X86_FEATURE_HYPERVISOR) ||
	    to_vmx(vcpu)->exit_reason.basic != EXIT_REASON_EPT_MISCONFIG) {
		orig_rip = kvm_rip_read(vcpu);
		rip = orig_rip + vmcs_read32(VM_EXIT_INSTRUCTION_LEN);
#ifdef CONFIG_X86_64
		/*
		 * We need to mask out the high 32 bits of RIP if not in 64-bit
		 * mode, but just finding out that we are in 64-bit mode is
		 * quite expensive.  Only do it if there was a carry.
		 */
		if (unlikely(((rip ^ orig_rip) >> 31) == 3) && !is_64_bit_mode(vcpu))
			rip = (u32)rip;
#endif
		kvm_rip_write(vcpu, rip);
	} else {
		if (!kvm_emulate_instruction(vcpu, EMULTYPE_SKIP))
			return 0;
	}

	/* skipping an emulated instruction also counts */
	vmx_set_interrupt_shadow(vcpu, 0);

	return 1;
}