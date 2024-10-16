static inline int cpu_has_vmx_invept_context(void)
{
	return (!!(vmx_capability.ept & VMX_EPT_EXTENT_CONTEXT_BIT));
}