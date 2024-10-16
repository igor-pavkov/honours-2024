static inline bool pt_output_base_valid(struct kvm_vcpu *vcpu, u64 base)
{
	/* The base must be 128-byte aligned and a legal physical address. */
	return kvm_vcpu_is_legal_aligned_gpa(vcpu, base, 128);
}