static int vmx_get_max_tdp_level(void)
{
	if (cpu_has_vmx_ept_5levels())
		return 5;
	return 4;
}