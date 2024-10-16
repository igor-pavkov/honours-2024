static inline int rdmsrl_amd_safe(unsigned msr, unsigned long long *p)
{
	u32 gprs[8] = { 0 };
	int err;

	WARN_ONCE((boot_cpu_data.x86 != 0xf),
		  "%s should only be used on K8!\n", __func__);

	gprs[1] = msr;
	gprs[7] = 0x9c5a203a;

	err = rdmsr_safe_regs(gprs);

	*p = gprs[0] | ((u64)gprs[2] << 32);

	return err;
}