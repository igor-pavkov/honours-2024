static void update_ia32_tsc_adjust_msr(struct kvm_vcpu *vcpu, s64 offset)
{
	u64 curr_offset = kvm_x86_ops->read_tsc_offset(vcpu);
	vcpu->arch.ia32_tsc_adjust_msr += offset - curr_offset;
}