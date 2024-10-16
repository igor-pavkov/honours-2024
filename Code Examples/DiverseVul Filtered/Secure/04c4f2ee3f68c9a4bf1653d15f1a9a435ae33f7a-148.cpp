static int vmx_rtit_ctl_check(struct kvm_vcpu *vcpu, u64 data)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long value;

	/*
	 * Any MSR write that attempts to change bits marked reserved will
	 * case a #GP fault.
	 */
	if (data & vmx->pt_desc.ctl_bitmask)
		return 1;

	/*
	 * Any attempt to modify IA32_RTIT_CTL while TraceEn is set will
	 * result in a #GP unless the same write also clears TraceEn.
	 */
	if ((vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) &&
		((vmx->pt_desc.guest.ctl ^ data) & ~RTIT_CTL_TRACEEN))
		return 1;

	/*
	 * WRMSR to IA32_RTIT_CTL that sets TraceEn but clears this bit
	 * and FabricEn would cause #GP, if
	 * CPUID.(EAX=14H, ECX=0):ECX.SNGLRGNOUT[bit 2] = 0
	 */
	if ((data & RTIT_CTL_TRACEEN) && !(data & RTIT_CTL_TOPA) &&
		!(data & RTIT_CTL_FABRIC_EN) &&
		!intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_single_range_output))
		return 1;

	/*
	 * MTCFreq, CycThresh and PSBFreq encodings check, any MSR write that
	 * utilize encodings marked reserved will casue a #GP fault.
	 */
	value = intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_mtc_periods);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_mtc) &&
			!test_bit((data & RTIT_CTL_MTC_RANGE) >>
			RTIT_CTL_MTC_RANGE_OFFSET, &value))
		return 1;
	value = intel_pt_validate_cap(vmx->pt_desc.caps,
						PT_CAP_cycle_thresholds);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_cyc) &&
			!test_bit((data & RTIT_CTL_CYC_THRESH) >>
			RTIT_CTL_CYC_THRESH_OFFSET, &value))
		return 1;
	value = intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_periods);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_cyc) &&
			!test_bit((data & RTIT_CTL_PSB_FREQ) >>
			RTIT_CTL_PSB_FREQ_OFFSET, &value))
		return 1;

	/*
	 * If ADDRx_CFG is reserved or the encodings is >2 will
	 * cause a #GP fault.
	 */
	value = (data & RTIT_CTL_ADDR0) >> RTIT_CTL_ADDR0_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 1)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR1) >> RTIT_CTL_ADDR1_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 2)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR2) >> RTIT_CTL_ADDR2_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 3)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR3) >> RTIT_CTL_ADDR3_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 4)) || (value > 2))
		return 1;

	return 0;
}