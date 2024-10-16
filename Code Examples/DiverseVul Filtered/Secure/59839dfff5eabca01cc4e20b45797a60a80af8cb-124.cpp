static void do_cpuid_ent(struct kvm_cpuid_entry2 *entry, u32 function,
			 u32 index, int *nent, int maxnent)
{
	const u32 kvm_supported_word0_x86_features = bit(X86_FEATURE_FPU) |
		bit(X86_FEATURE_VME) | bit(X86_FEATURE_DE) |
		bit(X86_FEATURE_PSE) | bit(X86_FEATURE_TSC) |
		bit(X86_FEATURE_MSR) | bit(X86_FEATURE_PAE) |
		bit(X86_FEATURE_CX8) | bit(X86_FEATURE_APIC) |
		bit(X86_FEATURE_SEP) | bit(X86_FEATURE_PGE) |
		bit(X86_FEATURE_CMOV) | bit(X86_FEATURE_PSE36) |
		bit(X86_FEATURE_CLFLSH) | bit(X86_FEATURE_MMX) |
		bit(X86_FEATURE_FXSR) | bit(X86_FEATURE_XMM) |
		bit(X86_FEATURE_XMM2) | bit(X86_FEATURE_SELFSNOOP);
	const u32 kvm_supported_word1_x86_features = bit(X86_FEATURE_FPU) |
		bit(X86_FEATURE_VME) | bit(X86_FEATURE_DE) |
		bit(X86_FEATURE_PSE) | bit(X86_FEATURE_TSC) |
		bit(X86_FEATURE_MSR) | bit(X86_FEATURE_PAE) |
		bit(X86_FEATURE_CX8) | bit(X86_FEATURE_APIC) |
		bit(X86_FEATURE_PGE) |
		bit(X86_FEATURE_CMOV) | bit(X86_FEATURE_PSE36) |
		bit(X86_FEATURE_MMX) | bit(X86_FEATURE_FXSR) |
		bit(X86_FEATURE_SYSCALL) |
		(is_efer_nx() ? bit(X86_FEATURE_NX) : 0) |
#ifdef CONFIG_X86_64
		bit(X86_FEATURE_LM) |
#endif
		bit(X86_FEATURE_FXSR_OPT) |
		bit(X86_FEATURE_MMXEXT) |
		bit(X86_FEATURE_3DNOWEXT) |
		bit(X86_FEATURE_3DNOW);
	const u32 kvm_supported_word3_x86_features =
		bit(X86_FEATURE_XMM3) | bit(X86_FEATURE_CX16);
	const u32 kvm_supported_word6_x86_features =
		bit(X86_FEATURE_LAHF_LM) | bit(X86_FEATURE_CMP_LEGACY) |
		bit(X86_FEATURE_SVM);

	/* all calls to cpuid_count() should be made on the same cpu */
	get_cpu();
	do_cpuid_1_ent(entry, function, index);
	++*nent;

	switch (function) {
	case 0:
		entry->eax = min(entry->eax, (u32)0xb);
		break;
	case 1:
		entry->edx &= kvm_supported_word0_x86_features;
		entry->ecx &= kvm_supported_word3_x86_features;
		break;
	/* function 2 entries are STATEFUL. That is, repeated cpuid commands
	 * may return different values. This forces us to get_cpu() before
	 * issuing the first command, and also to emulate this annoying behavior
	 * in kvm_emulate_cpuid() using KVM_CPUID_FLAG_STATE_READ_NEXT */
	case 2: {
		int t, times = entry->eax & 0xff;

		entry->flags |= KVM_CPUID_FLAG_STATEFUL_FUNC;
		entry->flags |= KVM_CPUID_FLAG_STATE_READ_NEXT;
		for (t = 1; t < times && *nent < maxnent; ++t) {
			do_cpuid_1_ent(&entry[t], function, 0);
			entry[t].flags |= KVM_CPUID_FLAG_STATEFUL_FUNC;
			++*nent;
		}
		break;
	}
	/* function 4 and 0xb have additional index. */
	case 4: {
		int i, cache_type;

		entry->flags |= KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
		/* read more entries until cache_type is zero */
		for (i = 1; *nent < maxnent; ++i) {
			cache_type = entry[i - 1].eax & 0x1f;
			if (!cache_type)
				break;
			do_cpuid_1_ent(&entry[i], function, i);
			entry[i].flags |=
			       KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
			++*nent;
		}
		break;
	}
	case 0xb: {
		int i, level_type;

		entry->flags |= KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
		/* read more entries until level_type is zero */
		for (i = 1; *nent < maxnent; ++i) {
			level_type = entry[i - 1].ecx & 0xff00;
			if (!level_type)
				break;
			do_cpuid_1_ent(&entry[i], function, i);
			entry[i].flags |=
			       KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
			++*nent;
		}
		break;
	}
	case 0x80000000:
		entry->eax = min(entry->eax, 0x8000001a);
		break;
	case 0x80000001:
		entry->edx &= kvm_supported_word1_x86_features;
		entry->ecx &= kvm_supported_word6_x86_features;
		break;
	}
	put_cpu();
}