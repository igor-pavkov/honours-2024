static bool cpu_has_sgx(void)
{
	return cpuid_eax(0) >= 0x12 && (cpuid_eax(0x12) & BIT(0));
}