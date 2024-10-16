static bool update_transition_efer(struct vcpu_vmx *vmx)
{
	u64 guest_efer = vmx->vcpu.arch.efer;
	u64 ignore_bits = 0;
	int i;

	/* Shadow paging assumes NX to be available.  */
	if (!enable_ept)
		guest_efer |= EFER_NX;

	/*
	 * LMA and LME handled by hardware; SCE meaningless outside long mode.
	 */
	ignore_bits |= EFER_SCE;
#ifdef CONFIG_X86_64
	ignore_bits |= EFER_LMA | EFER_LME;
	/* SCE is meaningful only in long mode on Intel */
	if (guest_efer & EFER_LMA)
		ignore_bits &= ~(u64)EFER_SCE;
#endif

	/*
	 * On EPT, we can't emulate NX, so we must switch EFER atomically.
	 * On CPUs that support "load IA32_EFER", always switch EFER
	 * atomically, since it's faster than switching it manually.
	 */
	if (cpu_has_load_ia32_efer() ||
	    (enable_ept && ((vmx->vcpu.arch.efer ^ host_efer) & EFER_NX))) {
		if (!(guest_efer & EFER_LMA))
			guest_efer &= ~EFER_LME;
		if (guest_efer != host_efer)
			add_atomic_switch_msr(vmx, MSR_EFER,
					      guest_efer, host_efer, false);
		else
			clear_atomic_switch_msr(vmx, MSR_EFER);
		return false;
	}

	i = __vmx_find_uret_msr(vmx, MSR_EFER);
	if (i < 0)
		return false;

	clear_atomic_switch_msr(vmx, MSR_EFER);

	guest_efer &= ~ignore_bits;
	guest_efer |= host_efer & ignore_bits;

	vmx->guest_uret_msrs[i].data = guest_efer;
	vmx->guest_uret_msrs[i].mask = ~ignore_bits;

	return true;
}