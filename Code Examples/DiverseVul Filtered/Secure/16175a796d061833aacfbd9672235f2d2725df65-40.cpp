static inline int vm_need_ept(void)
{
	return (cpu_has_vmx_ept() && enable_ept);
}