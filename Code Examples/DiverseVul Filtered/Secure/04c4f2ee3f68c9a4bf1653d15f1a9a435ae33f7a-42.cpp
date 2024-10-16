static int handle_invlpg(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmx_get_exit_qual(vcpu);

	kvm_mmu_invlpg(vcpu, exit_qualification);
	return kvm_skip_emulated_instruction(vcpu);
}