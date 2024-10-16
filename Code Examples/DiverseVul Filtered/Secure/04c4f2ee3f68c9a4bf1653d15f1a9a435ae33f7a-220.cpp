static void vmcs_set_secondary_exec_control(struct vcpu_vmx *vmx)
{
	/*
	 * These bits in the secondary execution controls field
	 * are dynamic, the others are mostly based on the hypervisor
	 * architecture and the guest's CPUID.  Do not touch the
	 * dynamic bits.
	 */
	u32 mask =
		SECONDARY_EXEC_SHADOW_VMCS |
		SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE |
		SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
		SECONDARY_EXEC_DESC;

	u32 new_ctl = vmx->secondary_exec_control;
	u32 cur_ctl = secondary_exec_controls_get(vmx);

	secondary_exec_controls_set(vmx, (new_ctl & ~mask) | (cur_ctl & mask));
}