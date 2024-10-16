static void load_transition_efer(struct vcpu_vmx *vmx)
{
	int efer_offset = vmx->msr_offset_efer;
	u64 host_efer = vmx->host_msrs[efer_offset].data;
	u64 guest_efer = vmx->guest_msrs[efer_offset].data;
	u64 ignore_bits;

	if (efer_offset < 0)
		return;
	/*
	 * NX is emulated; LMA and LME handled by hardware; SCE meaninless
	 * outside long mode
	 */
	ignore_bits = EFER_NX | EFER_SCE;
#ifdef CONFIG_X86_64
	ignore_bits |= EFER_LMA | EFER_LME;
	/* SCE is meaningful only in long mode on Intel */
	if (guest_efer & EFER_LMA)
		ignore_bits &= ~(u64)EFER_SCE;
#endif
	if ((guest_efer & ~ignore_bits) == (host_efer & ~ignore_bits))
		return;

	vmx->host_state.guest_efer_loaded = 1;
	guest_efer &= ~ignore_bits;
	guest_efer |= host_efer & ignore_bits;
	wrmsrl(MSR_EFER, guest_efer);
	vmx->vcpu.stat.efer_reload++;
}