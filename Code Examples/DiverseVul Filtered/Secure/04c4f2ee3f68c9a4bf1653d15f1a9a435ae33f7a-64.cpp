static void vmx_setup_uret_msr(struct vcpu_vmx *vmx, unsigned int msr)
{
	struct vmx_uret_msr tmp;
	int from, to;

	from = __vmx_find_uret_msr(vmx, msr);
	if (from < 0)
		return;
	to = vmx->nr_active_uret_msrs++;

	tmp = vmx->guest_uret_msrs[to];
	vmx->guest_uret_msrs[to] = vmx->guest_uret_msrs[from];
	vmx->guest_uret_msrs[from] = tmp;
}