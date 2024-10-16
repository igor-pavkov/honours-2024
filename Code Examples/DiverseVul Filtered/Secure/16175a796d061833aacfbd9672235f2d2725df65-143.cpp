static void skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	unsigned long rip;
	u32 interruptibility;

	rip = kvm_rip_read(vcpu);
	rip += vmcs_read32(VM_EXIT_INSTRUCTION_LEN);
	kvm_rip_write(vcpu, rip);

	/*
	 * We emulated an instruction, so temporary interrupt blocking
	 * should be removed, if set.
	 */
	interruptibility = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO);
	if (interruptibility & 3)
		vmcs_write32(GUEST_INTERRUPTIBILITY_INFO,
			     interruptibility & ~3);
	vcpu->arch.interrupt_window_open = 1;
}