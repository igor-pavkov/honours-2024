static u64 vmx_get_mt_mask(struct kvm_vcpu *vcpu, gfn_t gfn, bool is_mmio)
{
	u8 cache;
	u64 ipat = 0;

	/* We wanted to honor guest CD/MTRR/PAT, but doing so could result in
	 * memory aliases with conflicting memory types and sometimes MCEs.
	 * We have to be careful as to what are honored and when.
	 *
	 * For MMIO, guest CD/MTRR are ignored.  The EPT memory type is set to
	 * UC.  The effective memory type is UC or WC depending on guest PAT.
	 * This was historically the source of MCEs and we want to be
	 * conservative.
	 *
	 * When there is no need to deal with noncoherent DMA (e.g., no VT-d
	 * or VT-d has snoop control), guest CD/MTRR/PAT are all ignored.  The
	 * EPT memory type is set to WB.  The effective memory type is forced
	 * WB.
	 *
	 * Otherwise, we trust guest.  Guest CD/MTRR/PAT are all honored.  The
	 * EPT memory type is used to emulate guest CD/MTRR.
	 */

	if (is_mmio) {
		cache = MTRR_TYPE_UNCACHABLE;
		goto exit;
	}

	if (!kvm_arch_has_noncoherent_dma(vcpu->kvm)) {
		ipat = VMX_EPT_IPAT_BIT;
		cache = MTRR_TYPE_WRBACK;
		goto exit;
	}

	if (kvm_read_cr0(vcpu) & X86_CR0_CD) {
		ipat = VMX_EPT_IPAT_BIT;
		if (kvm_check_has_quirk(vcpu->kvm, KVM_X86_QUIRK_CD_NW_CLEARED))
			cache = MTRR_TYPE_WRBACK;
		else
			cache = MTRR_TYPE_UNCACHABLE;
		goto exit;
	}

	cache = kvm_mtrr_get_guest_memory_type(vcpu, gfn);

exit:
	return (cache << VMX_EPT_MT_EPTE_SHIFT) | ipat;
}