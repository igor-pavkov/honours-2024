static void kvm_init_msr_list(void)
{
	u32 dummy[2];
	unsigned i, j;

	for (i = j = 0; i < ARRAY_SIZE(msrs_to_save); i++) {
		if (rdmsr_safe(msrs_to_save[i], &dummy[0], &dummy[1]) < 0)
			continue;

		/*
		 * Even MSRs that are valid in the host may not be exposed
		 * to the guests in some cases.
		 */
		switch (msrs_to_save[i]) {
		case MSR_IA32_BNDCFGS:
			if (!kvm_mpx_supported())
				continue;
			break;
		case MSR_TSC_AUX:
			if (!kvm_x86_ops->rdtscp_supported())
				continue;
			break;
		default:
			break;
		}

		if (j < i)
			msrs_to_save[j] = msrs_to_save[i];
		j++;
	}
	num_msrs_to_save = j;

	for (i = j = 0; i < ARRAY_SIZE(emulated_msrs); i++) {
		if (!kvm_x86_ops->has_emulated_msr(emulated_msrs[i]))
			continue;

		if (j < i)
			emulated_msrs[j] = emulated_msrs[i];
		j++;
	}
	num_emulated_msrs = j;

	for (i = j = 0; i < ARRAY_SIZE(msr_based_features); i++) {
		struct kvm_msr_entry msr;

		msr.index = msr_based_features[i];
		if (kvm_get_msr_feature(&msr))
			continue;

		if (j < i)
			msr_based_features[j] = msr_based_features[i];
		j++;
	}
	num_msr_based_features = j;
}