static int handle_wrmsr(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	u32 ecx = vcpu->arch.regs[VCPU_REGS_RCX];
	u64 data = (vcpu->arch.regs[VCPU_REGS_RAX] & -1u)
		| ((u64)(vcpu->arch.regs[VCPU_REGS_RDX] & -1u) << 32);

	KVMTRACE_3D(MSR_WRITE, vcpu, ecx, (u32)data, (u32)(data >> 32),
		    handler);

	if (vmx_set_msr(vcpu, ecx, data) != 0) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	skip_emulated_instruction(vcpu);
	return 1;
}