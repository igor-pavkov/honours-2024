int emulate_invlpg(struct kvm_vcpu *vcpu, gva_t address)
{
	kvm_mmu_invlpg(vcpu, address);
	return X86EMUL_CONTINUE;
}