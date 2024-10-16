static inline int wrmsrl_amd_safe(unsigned msr, unsigned long long val)
{
	u32 gprs[8] = { 0 };

	WARN_ONCE((boot_cpu_data.x86 != 0xf),
		  "%s should only be used on K8!\n", __func__);

	gprs[0] = (u32)val;
	gprs[1] = msr;
	gprs[2] = val >> 32;
	gprs[7] = 0x9c5a203a;

	return wrmsr_safe_regs(gprs);
}