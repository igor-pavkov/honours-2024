void kvm_lmsw(struct kvm_vcpu *vcpu, unsigned long msw)
{
	kvm_set_cr0(vcpu, (vcpu->arch.cr0 & ~0x0ful) | (msw & 0x0f));
	KVMTRACE_1D(LMSW, vcpu,
		    (u32)((vcpu->arch.cr0 & ~0x0ful) | (msw & 0x0f)),
		    handler);
}