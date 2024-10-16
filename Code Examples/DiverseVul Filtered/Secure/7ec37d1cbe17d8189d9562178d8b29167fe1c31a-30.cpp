int kvm_hv_hypercall(struct kvm_vcpu *vcpu)
{
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);
	struct kvm_hv_hcall hc;
	u64 ret = HV_STATUS_SUCCESS;

	/*
	 * hypercall generates UD from non zero cpl and real mode
	 * per HYPER-V spec
	 */
	if (static_call(kvm_x86_get_cpl)(vcpu) != 0 || !is_protmode(vcpu)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

#ifdef CONFIG_X86_64
	if (is_64_bit_hypercall(vcpu)) {
		hc.param = kvm_rcx_read(vcpu);
		hc.ingpa = kvm_rdx_read(vcpu);
		hc.outgpa = kvm_r8_read(vcpu);
	} else
#endif
	{
		hc.param = ((u64)kvm_rdx_read(vcpu) << 32) |
			    (kvm_rax_read(vcpu) & 0xffffffff);
		hc.ingpa = ((u64)kvm_rbx_read(vcpu) << 32) |
			    (kvm_rcx_read(vcpu) & 0xffffffff);
		hc.outgpa = ((u64)kvm_rdi_read(vcpu) << 32) |
			     (kvm_rsi_read(vcpu) & 0xffffffff);
	}

	hc.code = hc.param & 0xffff;
	hc.var_cnt = (hc.param & HV_HYPERCALL_VARHEAD_MASK) >> HV_HYPERCALL_VARHEAD_OFFSET;
	hc.fast = !!(hc.param & HV_HYPERCALL_FAST_BIT);
	hc.rep_cnt = (hc.param >> HV_HYPERCALL_REP_COMP_OFFSET) & 0xfff;
	hc.rep_idx = (hc.param >> HV_HYPERCALL_REP_START_OFFSET) & 0xfff;
	hc.rep = !!(hc.rep_cnt || hc.rep_idx);

	trace_kvm_hv_hypercall(hc.code, hc.fast, hc.var_cnt, hc.rep_cnt,
			       hc.rep_idx, hc.ingpa, hc.outgpa);

	if (unlikely(!hv_check_hypercall_access(hv_vcpu, hc.code))) {
		ret = HV_STATUS_ACCESS_DENIED;
		goto hypercall_complete;
	}

	if (unlikely(hc.param & HV_HYPERCALL_RSVD_MASK)) {
		ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
		goto hypercall_complete;
	}

	if (hc.fast && is_xmm_fast_hypercall(&hc)) {
		if (unlikely(hv_vcpu->enforce_cpuid &&
			     !(hv_vcpu->cpuid_cache.features_edx &
			       HV_X64_HYPERCALL_XMM_INPUT_AVAILABLE))) {
			kvm_queue_exception(vcpu, UD_VECTOR);
			return 1;
		}

		kvm_hv_hypercall_read_xmm(&hc);
	}

	switch (hc.code) {
	case HVCALL_NOTIFY_LONG_SPIN_WAIT:
		if (unlikely(hc.rep || hc.var_cnt)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		kvm_vcpu_on_spin(vcpu, true);
		break;
	case HVCALL_SIGNAL_EVENT:
		if (unlikely(hc.rep || hc.var_cnt)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hvcall_signal_event(vcpu, &hc);
		if (ret != HV_STATUS_INVALID_PORT_ID)
			break;
		fallthrough;	/* maybe userspace knows this conn_id */
	case HVCALL_POST_MESSAGE:
		/* don't bother userspace if it has no way to handle it */
		if (unlikely(hc.rep || hc.var_cnt || !to_hv_synic(vcpu)->active)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		vcpu->run->exit_reason = KVM_EXIT_HYPERV;
		vcpu->run->hyperv.type = KVM_EXIT_HYPERV_HCALL;
		vcpu->run->hyperv.u.hcall.input = hc.param;
		vcpu->run->hyperv.u.hcall.params[0] = hc.ingpa;
		vcpu->run->hyperv.u.hcall.params[1] = hc.outgpa;
		vcpu->arch.complete_userspace_io =
				kvm_hv_hypercall_complete_userspace;
		return 0;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST:
		if (unlikely(hc.var_cnt)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		fallthrough;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST_EX:
		if (unlikely(!hc.rep_cnt || hc.rep_idx)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, &hc);
		break;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE:
		if (unlikely(hc.var_cnt)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		fallthrough;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE_EX:
		if (unlikely(hc.rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, &hc);
		break;
	case HVCALL_SEND_IPI:
		if (unlikely(hc.var_cnt)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		fallthrough;
	case HVCALL_SEND_IPI_EX:
		if (unlikely(hc.rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_send_ipi(vcpu, &hc);
		break;
	case HVCALL_POST_DEBUG_DATA:
	case HVCALL_RETRIEVE_DEBUG_DATA:
		if (unlikely(hc.fast)) {
			ret = HV_STATUS_INVALID_PARAMETER;
			break;
		}
		fallthrough;
	case HVCALL_RESET_DEBUG_SESSION: {
		struct kvm_hv_syndbg *syndbg = to_hv_syndbg(vcpu);

		if (!kvm_hv_is_syndbg_enabled(vcpu)) {
			ret = HV_STATUS_INVALID_HYPERCALL_CODE;
			break;
		}

		if (!(syndbg->options & HV_X64_SYNDBG_OPTION_USE_HCALLS)) {
			ret = HV_STATUS_OPERATION_DENIED;
			break;
		}
		vcpu->run->exit_reason = KVM_EXIT_HYPERV;
		vcpu->run->hyperv.type = KVM_EXIT_HYPERV_HCALL;
		vcpu->run->hyperv.u.hcall.input = hc.param;
		vcpu->run->hyperv.u.hcall.params[0] = hc.ingpa;
		vcpu->run->hyperv.u.hcall.params[1] = hc.outgpa;
		vcpu->arch.complete_userspace_io =
				kvm_hv_hypercall_complete_userspace;
		return 0;
	}
	default:
		ret = HV_STATUS_INVALID_HYPERCALL_CODE;
		break;
	}

hypercall_complete:
	return kvm_hv_hypercall_complete(vcpu, ret);
}