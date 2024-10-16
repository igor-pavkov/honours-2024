static inline int cpu_has_vmx_invept_global(void)
{
	return (!!(vmx_capability.ept & VMX_EPT_EXTENT_GLOBAL_BIT));
}