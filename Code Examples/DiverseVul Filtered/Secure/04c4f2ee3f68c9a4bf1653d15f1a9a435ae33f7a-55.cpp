static void add_atomic_switch_msr(struct vcpu_vmx *vmx, unsigned msr,
				  u64 guest_val, u64 host_val, bool entry_only)
{
	int i, j = 0;
	struct msr_autoload *m = &vmx->msr_autoload;

	switch (msr) {
	case MSR_EFER:
		if (cpu_has_load_ia32_efer()) {
			add_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_EFER,
					VM_EXIT_LOAD_IA32_EFER,
					GUEST_IA32_EFER,
					HOST_IA32_EFER,
					guest_val, host_val);
			return;
		}
		break;
	case MSR_CORE_PERF_GLOBAL_CTRL:
		if (cpu_has_load_perf_global_ctrl()) {
			add_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL,
					VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL,
					GUEST_IA32_PERF_GLOBAL_CTRL,
					HOST_IA32_PERF_GLOBAL_CTRL,
					guest_val, host_val);
			return;
		}
		break;
	case MSR_IA32_PEBS_ENABLE:
		/* PEBS needs a quiescent period after being disabled (to write
		 * a record).  Disabling PEBS through VMX MSR swapping doesn't
		 * provide that period, so a CPU could write host's record into
		 * guest's memory.
		 */
		wrmsrl(MSR_IA32_PEBS_ENABLE, 0);
	}

	i = vmx_find_loadstore_msr_slot(&m->guest, msr);
	if (!entry_only)
		j = vmx_find_loadstore_msr_slot(&m->host, msr);

	if ((i < 0 && m->guest.nr == MAX_NR_LOADSTORE_MSRS) ||
	    (j < 0 &&  m->host.nr == MAX_NR_LOADSTORE_MSRS)) {
		printk_once(KERN_WARNING "Not enough msr switch entries. "
				"Can't add msr %x\n", msr);
		return;
	}
	if (i < 0) {
		i = m->guest.nr++;
		vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, m->guest.nr);
	}
	m->guest.val[i].index = msr;
	m->guest.val[i].value = guest_val;

	if (entry_only)
		return;

	if (j < 0) {
		j = m->host.nr++;
		vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, m->host.nr);
	}
	m->host.val[j].index = msr;
	m->host.val[j].value = host_val;
}