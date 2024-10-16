static inline void ept_sync_context(u64 eptp)
{
	if (vm_need_ept()) {
		if (cpu_has_vmx_invept_context())
			__invept(VMX_EPT_EXTENT_CONTEXT, eptp, 0);
		else
			ept_sync_global();
	}
}