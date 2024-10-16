static int handle_invd(struct kvm_vcpu *vcpu)
{
	/* Treat an INVD instruction as a NOP and just skip it. */
	return kvm_skip_emulated_instruction(vcpu);
}