static int vmx_set_msr(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmx_uret_msr *msr;
	int ret = 0;
	u32 msr_index = msr_info->index;
	u64 data = msr_info->data;
	u32 index;

	switch (msr_index) {
	case MSR_EFER:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
#ifdef CONFIG_X86_64
	case MSR_FS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_FS_BASE, data);
		break;
	case MSR_GS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_GS_BASE, data);
		break;
	case MSR_KERNEL_GS_BASE:
		vmx_write_guest_kernel_gs_base(vmx, data);
		break;
#endif
	case MSR_IA32_SYSENTER_CS:
		if (is_guest_mode(vcpu))
			get_vmcs12(vcpu)->guest_sysenter_cs = data;
		vmcs_write32(GUEST_SYSENTER_CS, data);
		break;
	case MSR_IA32_SYSENTER_EIP:
		if (is_guest_mode(vcpu)) {
			data = nested_vmx_truncate_sysenter_addr(vcpu, data);
			get_vmcs12(vcpu)->guest_sysenter_eip = data;
		}
		vmcs_writel(GUEST_SYSENTER_EIP, data);
		break;
	case MSR_IA32_SYSENTER_ESP:
		if (is_guest_mode(vcpu)) {
			data = nested_vmx_truncate_sysenter_addr(vcpu, data);
			get_vmcs12(vcpu)->guest_sysenter_esp = data;
		}
		vmcs_writel(GUEST_SYSENTER_ESP, data);
		break;
	case MSR_IA32_DEBUGCTLMSR: {
		u64 invalid = data & ~vcpu_supported_debugctl(vcpu);
		if (invalid & (DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR)) {
			if (report_ignored_msrs)
				vcpu_unimpl(vcpu, "%s: BTF|LBR in IA32_DEBUGCTLMSR 0x%llx, nop\n",
					    __func__, data);
			data &= ~(DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR);
			invalid &= ~(DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR);
		}

		if (invalid)
			return 1;

		if (is_guest_mode(vcpu) && get_vmcs12(vcpu)->vm_exit_controls &
						VM_EXIT_SAVE_DEBUG_CONTROLS)
			get_vmcs12(vcpu)->guest_ia32_debugctl = data;

		vmcs_write64(GUEST_IA32_DEBUGCTL, data);
		if (intel_pmu_lbr_is_enabled(vcpu) && !to_vmx(vcpu)->lbr_desc.event &&
		    (data & DEBUGCTLMSR_LBR))
			intel_pmu_create_guest_lbr_event(vcpu);
		return 0;
	}
	case MSR_IA32_BNDCFGS:
		if (!kvm_mpx_supported() ||
		    (!msr_info->host_initiated &&
		     !guest_cpuid_has(vcpu, X86_FEATURE_MPX)))
			return 1;
		if (is_noncanonical_address(data & PAGE_MASK, vcpu) ||
		    (data & MSR_IA32_BNDCFGS_RSVD))
			return 1;
		vmcs_write64(GUEST_BNDCFGS, data);
		break;
	case MSR_IA32_UMWAIT_CONTROL:
		if (!msr_info->host_initiated && !vmx_has_waitpkg(vmx))
			return 1;

		/* The reserved bit 1 and non-32 bit [63:32] should be zero */
		if (data & (BIT_ULL(1) | GENMASK_ULL(63, 32)))
			return 1;

		vmx->msr_ia32_umwait_control = data;
		break;
	case MSR_IA32_SPEC_CTRL:
		if (!msr_info->host_initiated &&
		    !guest_has_spec_ctrl_msr(vcpu))
			return 1;

		if (kvm_spec_ctrl_test_value(data))
			return 1;

		vmx->spec_ctrl = data;
		if (!data)
			break;

		/*
		 * For non-nested:
		 * When it's written (to non-zero) for the first time, pass
		 * it through.
		 *
		 * For nested:
		 * The handling of the MSR bitmap for L2 guests is done in
		 * nested_vmx_prepare_msr_bitmap. We should not touch the
		 * vmcs02.msr_bitmap here since it gets completely overwritten
		 * in the merging. We update the vmcs01 here for L1 as well
		 * since it will end up touching the MSR anyway now.
		 */
		vmx_disable_intercept_for_msr(vcpu,
					      MSR_IA32_SPEC_CTRL,
					      MSR_TYPE_RW);
		break;
	case MSR_IA32_TSX_CTRL:
		if (!msr_info->host_initiated &&
		    !(vcpu->arch.arch_capabilities & ARCH_CAP_TSX_CTRL_MSR))
			return 1;
		if (data & ~(TSX_CTRL_RTM_DISABLE | TSX_CTRL_CPUID_CLEAR))
			return 1;
		goto find_uret_msr;
	case MSR_IA32_PRED_CMD:
		if (!msr_info->host_initiated &&
		    !guest_has_pred_cmd_msr(vcpu))
			return 1;

		if (data & ~PRED_CMD_IBPB)
			return 1;
		if (!boot_cpu_has(X86_FEATURE_IBPB))
			return 1;
		if (!data)
			break;

		wrmsrl(MSR_IA32_PRED_CMD, PRED_CMD_IBPB);

		/*
		 * For non-nested:
		 * When it's written (to non-zero) for the first time, pass
		 * it through.
		 *
		 * For nested:
		 * The handling of the MSR bitmap for L2 guests is done in
		 * nested_vmx_prepare_msr_bitmap. We should not touch the
		 * vmcs02.msr_bitmap here since it gets completely overwritten
		 * in the merging.
		 */
		vmx_disable_intercept_for_msr(vcpu, MSR_IA32_PRED_CMD, MSR_TYPE_W);
		break;
	case MSR_IA32_CR_PAT:
		if (!kvm_pat_valid(data))
			return 1;

		if (is_guest_mode(vcpu) &&
		    get_vmcs12(vcpu)->vm_exit_controls & VM_EXIT_SAVE_IA32_PAT)
			get_vmcs12(vcpu)->guest_ia32_pat = data;

		if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT) {
			vmcs_write64(GUEST_IA32_PAT, data);
			vcpu->arch.pat = data;
			break;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_TSC_ADJUST:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_MCG_EXT_CTL:
		if ((!msr_info->host_initiated &&
		     !(to_vmx(vcpu)->msr_ia32_feature_control &
		       FEAT_CTL_LMCE_ENABLED)) ||
		    (data & ~MCG_EXT_CTL_LMCE_EN))
			return 1;
		vcpu->arch.mcg_ext_ctl = data;
		break;
	case MSR_IA32_FEAT_CTL:
		if (!vmx_feature_control_msr_valid(vcpu, data) ||
		    (to_vmx(vcpu)->msr_ia32_feature_control &
		     FEAT_CTL_LOCKED && !msr_info->host_initiated))
			return 1;
		vmx->msr_ia32_feature_control = data;
		if (msr_info->host_initiated && data == 0)
			vmx_leave_nested(vcpu);
		break;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!msr_info->host_initiated)
			return 1; /* they are read-only */
		if (!nested_vmx_allowed(vcpu))
			return 1;
		return vmx_set_vmx_msr(vcpu, msr_index, data);
	case MSR_IA32_RTIT_CTL:
		if (!vmx_pt_mode_is_host_guest() ||
			vmx_rtit_ctl_check(vcpu, data) ||
			vmx->nested.vmxon)
			return 1;
		vmcs_write64(GUEST_IA32_RTIT_CTL, data);
		vmx->pt_desc.guest.ctl = data;
		pt_update_intercept_for_msr(vcpu);
		break;
	case MSR_IA32_RTIT_STATUS:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (data & MSR_IA32_RTIT_STATUS_MASK)
			return 1;
		vmx->pt_desc.guest.status = data;
		break;
	case MSR_IA32_RTIT_CR3_MATCH:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_cr3_filtering))
			return 1;
		vmx->pt_desc.guest.cr3_match = data;
		break;
	case MSR_IA32_RTIT_OUTPUT_BASE:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_topa_output) &&
		    !intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_single_range_output))
			return 1;
		if (!pt_output_base_valid(vcpu, data))
			return 1;
		vmx->pt_desc.guest.output_base = data;
		break;
	case MSR_IA32_RTIT_OUTPUT_MASK:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_topa_output) &&
		    !intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_single_range_output))
			return 1;
		vmx->pt_desc.guest.output_mask = data;
		break;
	case MSR_IA32_RTIT_ADDR0_A ... MSR_IA32_RTIT_ADDR3_B:
		if (!pt_can_write_msr(vmx))
			return 1;
		index = msr_info->index - MSR_IA32_RTIT_ADDR0_A;
		if (index >= 2 * intel_pt_validate_cap(vmx->pt_desc.caps,
						       PT_CAP_num_address_ranges))
			return 1;
		if (is_noncanonical_address(data, vcpu))
			return 1;
		if (index % 2)
			vmx->pt_desc.guest.addr_b[index / 2] = data;
		else
			vmx->pt_desc.guest.addr_a[index / 2] = data;
		break;
	case MSR_TSC_AUX:
		if (!msr_info->host_initiated &&
		    !guest_cpuid_has(vcpu, X86_FEATURE_RDTSCP))
			return 1;
		/* Check reserved bit, higher 32 bits should be zero */
		if ((data >> 32) != 0)
			return 1;
		goto find_uret_msr;
	case MSR_IA32_PERF_CAPABILITIES:
		if (data && !vcpu_to_pmu(vcpu)->version)
			return 1;
		if (data & PMU_CAP_LBR_FMT) {
			if ((data & PMU_CAP_LBR_FMT) !=
			    (vmx_get_perf_capabilities() & PMU_CAP_LBR_FMT))
				return 1;
			if (!intel_pmu_lbr_is_compatible(vcpu))
				return 1;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;

	default:
	find_uret_msr:
		msr = vmx_find_uret_msr(vmx, msr_index);
		if (msr)
			ret = vmx_set_guest_uret_msr(vmx, msr, data);
		else
			ret = kvm_set_msr_common(vcpu, msr_info);
	}

	return ret;
}