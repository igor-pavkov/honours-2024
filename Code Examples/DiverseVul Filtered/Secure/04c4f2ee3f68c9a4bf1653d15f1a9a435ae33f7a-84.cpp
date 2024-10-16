static __init int alloc_kvm_area(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct vmcs *vmcs;

		vmcs = alloc_vmcs_cpu(false, cpu, GFP_KERNEL);
		if (!vmcs) {
			free_kvm_area();
			return -ENOMEM;
		}

		/*
		 * When eVMCS is enabled, alloc_vmcs_cpu() sets
		 * vmcs->revision_id to KVM_EVMCS_VERSION instead of
		 * revision_id reported by MSR_IA32_VMX_BASIC.
		 *
		 * However, even though not explicitly documented by
		 * TLFS, VMXArea passed as VMXON argument should
		 * still be marked with revision_id reported by
		 * physical CPU.
		 */
		if (static_branch_unlikely(&enable_evmcs))
			vmcs->hdr.revision_id = vmcs_config.revision_id;

		per_cpu(vmxarea, cpu) = vmcs;
	}
	return 0;
}