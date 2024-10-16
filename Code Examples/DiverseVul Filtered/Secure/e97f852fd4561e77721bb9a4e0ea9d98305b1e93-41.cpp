int kvm_emulate_instruction_from_buffer(struct kvm_vcpu *vcpu,
					void *insn, int insn_len)
{
	return x86_emulate_instruction(vcpu, 0, 0, insn, insn_len);
}