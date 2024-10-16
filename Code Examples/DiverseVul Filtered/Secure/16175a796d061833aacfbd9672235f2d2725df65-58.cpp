static void vmx_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u64 phys_addr = __pa(vmx->vmcs);
	u64 tsc_this, delta, new_offset;

	if (vcpu->cpu != cpu) {
		vcpu_clear(vmx);
		kvm_migrate_timers(vcpu);
		vpid_sync_vcpu_all(vmx);
		local_irq_disable();
		list_add(&vmx->local_vcpus_link,
			 &per_cpu(vcpus_on_cpu, cpu));
		local_irq_enable();
	}

	if (per_cpu(current_vmcs, cpu) != vmx->vmcs) {
		u8 error;

		per_cpu(current_vmcs, cpu) = vmx->vmcs;
		asm volatile (__ex(ASM_VMX_VMPTRLD_RAX) "; setna %0"
			      : "=g"(error) : "a"(&phys_addr), "m"(phys_addr)
			      : "cc");
		if (error)
			printk(KERN_ERR "kvm: vmptrld %p/%llx fail\n",
			       vmx->vmcs, phys_addr);
	}

	if (vcpu->cpu != cpu) {
		struct descriptor_table dt;
		unsigned long sysenter_esp;

		vcpu->cpu = cpu;
		/*
		 * Linux uses per-cpu TSS and GDT, so set these when switching
		 * processors.
		 */
		vmcs_writel(HOST_TR_BASE, kvm_read_tr_base()); /* 22.2.4 */
		kvm_get_gdt(&dt);
		vmcs_writel(HOST_GDTR_BASE, dt.base);   /* 22.2.4 */

		rdmsrl(MSR_IA32_SYSENTER_ESP, sysenter_esp);
		vmcs_writel(HOST_IA32_SYSENTER_ESP, sysenter_esp); /* 22.2.3 */

		/*
		 * Make sure the time stamp counter is monotonous.
		 */
		rdtscll(tsc_this);
		if (tsc_this < vcpu->arch.host_tsc) {
			delta = vcpu->arch.host_tsc - tsc_this;
			new_offset = vmcs_read64(TSC_OFFSET) + delta;
			vmcs_write64(TSC_OFFSET, new_offset);
		}
	}
}