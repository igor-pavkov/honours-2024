static void hardware_enable(void *garbage)
{
	int cpu = raw_smp_processor_id();
	u64 phys_addr = __pa(per_cpu(vmxarea, cpu));
	u64 old;

	INIT_LIST_HEAD(&per_cpu(vcpus_on_cpu, cpu));
	rdmsrl(MSR_IA32_FEATURE_CONTROL, old);
	if ((old & (FEATURE_CONTROL_LOCKED |
		    FEATURE_CONTROL_VMXON_ENABLED))
	    != (FEATURE_CONTROL_LOCKED |
		FEATURE_CONTROL_VMXON_ENABLED))
		/* enable and lock */
		wrmsrl(MSR_IA32_FEATURE_CONTROL, old |
		       FEATURE_CONTROL_LOCKED |
		       FEATURE_CONTROL_VMXON_ENABLED);
	write_cr4(read_cr4() | X86_CR4_VMXE); /* FIXME: not cpu hotplug safe */
	asm volatile (ASM_VMX_VMXON_RAX
		      : : "a"(&phys_addr), "m"(phys_addr)
		      : "memory", "cc");
}