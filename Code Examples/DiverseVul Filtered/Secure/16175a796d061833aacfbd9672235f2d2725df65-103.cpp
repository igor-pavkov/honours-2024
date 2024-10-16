static int handle_invlpg(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	u64 exit_qualification = vmcs_read64(EXIT_QUALIFICATION);

	kvm_mmu_invlpg(vcpu, exit_qualification);
	skip_emulated_instruction(vcpu);
	return 1;
}