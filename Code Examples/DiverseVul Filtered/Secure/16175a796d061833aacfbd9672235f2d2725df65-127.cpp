static inline void ept_sync_individual_addr(u64 eptp, gpa_t gpa)
{
	if (vm_need_ept()) {
		if (cpu_has_vmx_invept_individual_addr())
			__invept(VMX_EPT_EXTENT_INDIVIDUAL_ADDR,
					eptp, gpa);
		else
			ept_sync_context(eptp);
	}
}