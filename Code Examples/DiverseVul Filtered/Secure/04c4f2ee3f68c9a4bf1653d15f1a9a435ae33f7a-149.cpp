static int handle_xsetbv(struct kvm_vcpu *vcpu)
{
	u64 new_bv = kvm_read_edx_eax(vcpu);
	u32 index = kvm_rcx_read(vcpu);

	int err = kvm_set_xcr(vcpu, index, new_bv);
	return kvm_complete_insn_gp(vcpu, err);
}