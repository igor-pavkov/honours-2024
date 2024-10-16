static void kvm_hv_hypercall_read_xmm(struct kvm_hv_hcall *hc)
{
	int reg;

	kvm_fpu_get();
	for (reg = 0; reg < HV_HYPERCALL_MAX_XMM_REGISTERS; reg++)
		_kvm_read_sse_reg(reg, &hc->xmm[reg]);
	kvm_fpu_put();
}