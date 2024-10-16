static u64 vmx_write_l1_tsc_offset(struct kvm_vcpu *vcpu, u64 offset)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	u64 g_tsc_offset = 0;

	/*
	 * We're here if L1 chose not to trap WRMSR to TSC. According
	 * to the spec, this should set L1's TSC; The offset that L1
	 * set for L2 remains unchanged, and still needs to be added
	 * to the newly set TSC to get L2's TSC.
	 */
	if (is_guest_mode(vcpu) &&
	    (vmcs12->cpu_based_vm_exec_control & CPU_BASED_USE_TSC_OFFSETTING))
		g_tsc_offset = vmcs12->tsc_offset;

	trace_kvm_write_tsc_offset(vcpu->vcpu_id,
				   vcpu->arch.tsc_offset - g_tsc_offset,
				   offset);
	vmcs_write64(TSC_OFFSET, offset + g_tsc_offset);
	return offset + g_tsc_offset;
}