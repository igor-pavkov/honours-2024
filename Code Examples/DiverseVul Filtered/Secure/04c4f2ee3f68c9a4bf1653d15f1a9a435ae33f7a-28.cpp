struct vmx_uret_msr *vmx_find_uret_msr(struct vcpu_vmx *vmx, u32 msr)
{
	int i;

	i = __vmx_find_uret_msr(vmx, msr);
	if (i >= 0)
		return &vmx->guest_uret_msrs[i];
	return NULL;
}