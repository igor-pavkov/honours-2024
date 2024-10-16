static inline int cpu_has_vmx_invept_individual_addr(void)
{
	return (!!(vmx_capability.ept & VMX_EPT_EXTENT_INDIVIDUAL_BIT));
}