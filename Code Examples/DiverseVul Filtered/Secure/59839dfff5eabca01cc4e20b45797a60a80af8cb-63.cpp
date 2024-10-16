int emulate_clts(struct kvm_vcpu *vcpu)
{
	KVMTRACE_0D(CLTS, vcpu, handler);
	kvm_x86_ops->set_cr0(vcpu, vcpu->arch.cr0 & ~X86_CR0_TS);
	return X86EMUL_CONTINUE;
}