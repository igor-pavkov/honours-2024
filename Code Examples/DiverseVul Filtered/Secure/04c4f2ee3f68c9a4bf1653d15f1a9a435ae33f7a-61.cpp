static inline int __vmx_find_uret_msr(struct vcpu_vmx *vmx, u32 msr)
{
	int i;

	for (i = 0; i < vmx->nr_uret_msrs; ++i)
		if (vmx_uret_msrs_list[vmx->guest_uret_msrs[i].slot] == msr)
			return i;
	return -1;
}