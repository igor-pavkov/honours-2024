static bool hv_check_msr_access(struct kvm_vcpu_hv *hv_vcpu, u32 msr)
{
	if (!hv_vcpu->enforce_cpuid)
		return true;

	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
	case HV_X64_MSR_HYPERCALL:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_HYPERCALL_AVAILABLE;
	case HV_X64_MSR_VP_RUNTIME:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_VP_RUNTIME_AVAILABLE;
	case HV_X64_MSR_TIME_REF_COUNT:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_TIME_REF_COUNT_AVAILABLE;
	case HV_X64_MSR_VP_INDEX:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_VP_INDEX_AVAILABLE;
	case HV_X64_MSR_RESET:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_RESET_AVAILABLE;
	case HV_X64_MSR_REFERENCE_TSC:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_REFERENCE_TSC_AVAILABLE;
	case HV_X64_MSR_SCONTROL:
	case HV_X64_MSR_SVERSION:
	case HV_X64_MSR_SIEFP:
	case HV_X64_MSR_SIMP:
	case HV_X64_MSR_EOM:
	case HV_X64_MSR_SINT0 ... HV_X64_MSR_SINT15:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_SYNIC_AVAILABLE;
	case HV_X64_MSR_STIMER0_CONFIG:
	case HV_X64_MSR_STIMER1_CONFIG:
	case HV_X64_MSR_STIMER2_CONFIG:
	case HV_X64_MSR_STIMER3_CONFIG:
	case HV_X64_MSR_STIMER0_COUNT:
	case HV_X64_MSR_STIMER1_COUNT:
	case HV_X64_MSR_STIMER2_COUNT:
	case HV_X64_MSR_STIMER3_COUNT:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_SYNTIMER_AVAILABLE;
	case HV_X64_MSR_EOI:
	case HV_X64_MSR_ICR:
	case HV_X64_MSR_TPR:
	case HV_X64_MSR_VP_ASSIST_PAGE:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_MSR_APIC_ACCESS_AVAILABLE;
		break;
	case HV_X64_MSR_TSC_FREQUENCY:
	case HV_X64_MSR_APIC_FREQUENCY:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_ACCESS_FREQUENCY_MSRS;
	case HV_X64_MSR_REENLIGHTENMENT_CONTROL:
	case HV_X64_MSR_TSC_EMULATION_CONTROL:
	case HV_X64_MSR_TSC_EMULATION_STATUS:
		return hv_vcpu->cpuid_cache.features_eax &
			HV_ACCESS_REENLIGHTENMENT;
	case HV_X64_MSR_CRASH_P0 ... HV_X64_MSR_CRASH_P4:
	case HV_X64_MSR_CRASH_CTL:
		return hv_vcpu->cpuid_cache.features_edx &
			HV_FEATURE_GUEST_CRASH_MSR_AVAILABLE;
	case HV_X64_MSR_SYNDBG_OPTIONS:
	case HV_X64_MSR_SYNDBG_CONTROL ... HV_X64_MSR_SYNDBG_PENDING_BUFFER:
		return hv_vcpu->cpuid_cache.features_edx &
			HV_FEATURE_DEBUG_MSRS_AVAILABLE;
	default:
		break;
	}

	return false;
}