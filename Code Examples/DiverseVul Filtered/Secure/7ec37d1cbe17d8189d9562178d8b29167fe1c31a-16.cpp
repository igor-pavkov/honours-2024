static int kvm_hv_hypercall_complete(struct kvm_vcpu *vcpu, u64 result)
{
	trace_kvm_hv_hypercall_done(result);
	kvm_hv_hypercall_set_result(vcpu, result);
	++vcpu->stat.hypercalls;
	return kvm_skip_emulated_instruction(vcpu);
}