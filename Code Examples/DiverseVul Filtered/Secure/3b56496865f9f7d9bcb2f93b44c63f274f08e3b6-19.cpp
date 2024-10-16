static void init_amd(struct cpuinfo_x86 *c)
{
	u32 dummy;
	unsigned long long value;

#ifdef CONFIG_SMP
	/*
	 * Disable TLB flush filter by setting HWCR.FFDIS on K8
	 * bit 6 of msr C001_0015
	 *
	 * Errata 63 for SH-B3 steppings
	 * Errata 122 for all steppings (F+ have it disabled by default)
	 */
	if (c->x86 == 0xf) {
		rdmsrl(MSR_K7_HWCR, value);
		value |= 1 << 6;
		wrmsrl(MSR_K7_HWCR, value);
	}
#endif

	early_init_amd(c);

	/*
	 * Bit 31 in normal CPUID used for nonstandard 3DNow ID;
	 * 3DNow is IDd by bit 31 in extended CPUID (1*32+31) anyway
	 */
	clear_cpu_cap(c, 0*32+31);

#ifdef CONFIG_X86_64
	/* On C+ stepping K8 rep microcode works well for copy/memset */
	if (c->x86 == 0xf) {
		u32 level;

		level = cpuid_eax(1);
		if ((level >= 0x0f48 && level < 0x0f50) || level >= 0x0f58)
			set_cpu_cap(c, X86_FEATURE_REP_GOOD);

		/*
		 * Some BIOSes incorrectly force this feature, but only K8
		 * revision D (model = 0x14) and later actually support it.
		 * (AMD Erratum #110, docId: 25759).
		 */
		if (c->x86_model < 0x14 && cpu_has(c, X86_FEATURE_LAHF_LM)) {
			clear_cpu_cap(c, X86_FEATURE_LAHF_LM);
			if (!rdmsrl_amd_safe(0xc001100d, &value)) {
				value &= ~(1ULL << 32);
				wrmsrl_amd_safe(0xc001100d, value);
			}
		}

	}
	if (c->x86 >= 0x10)
		set_cpu_cap(c, X86_FEATURE_REP_GOOD);

	/* get apicid instead of initial apic id from cpuid */
	c->apicid = hard_smp_processor_id();
#else

	/*
	 *	FIXME: We should handle the K5 here. Set up the write
	 *	range and also turn on MSR 83 bits 4 and 31 (write alloc,
	 *	no bus pipeline)
	 */

	switch (c->x86) {
	case 4:
		init_amd_k5(c);
		break;
	case 5:
		init_amd_k6(c);
		break;
	case 6: /* An Athlon/Duron */
		init_amd_k7(c);
		break;
	}

	/* K6s reports MCEs but don't actually have all the MSRs */
	if (c->x86 < 6)
		clear_cpu_cap(c, X86_FEATURE_MCE);
#endif

	/* Enable workaround for FXSAVE leak */
	if (c->x86 >= 6)
		set_cpu_cap(c, X86_FEATURE_FXSAVE_LEAK);

	if (!c->x86_model_id[0]) {
		switch (c->x86) {
		case 0xf:
			/* Should distinguish Models here, but this is only
			   a fallback anyways. */
			strcpy(c->x86_model_id, "Hammer");
			break;
		}
	}

	/* re-enable TopologyExtensions if switched off by BIOS */
	if ((c->x86 == 0x15) &&
	    (c->x86_model >= 0x10) && (c->x86_model <= 0x1f) &&
	    !cpu_has(c, X86_FEATURE_TOPOEXT)) {

		if (!rdmsrl_safe(0xc0011005, &value)) {
			value |= 1ULL << 54;
			wrmsrl_safe(0xc0011005, value);
			rdmsrl(0xc0011005, value);
			if (value & (1ULL << 54)) {
				set_cpu_cap(c, X86_FEATURE_TOPOEXT);
				printk(KERN_INFO FW_INFO "CPU: Re-enabling "
				  "disabled Topology Extensions Support\n");
			}
		}
	}

	/*
	 * The way access filter has a performance penalty on some workloads.
	 * Disable it on the affected CPUs.
	 */
	if ((c->x86 == 0x15) &&
	    (c->x86_model >= 0x02) && (c->x86_model < 0x20)) {

		if (!rdmsrl_safe(0xc0011021, &value) && !(value & 0x1E)) {
			value |= 0x1E;
			wrmsrl_safe(0xc0011021, value);
		}
	}

	cpu_detect_cache_sizes(c);

	/* Multi core CPU? */
	if (c->extended_cpuid_level >= 0x80000008) {
		amd_detect_cmp(c);
		srat_detect_node(c);
	}

#ifdef CONFIG_X86_32
	detect_ht(c);
#endif

	init_amd_cacheinfo(c);

	if (c->x86 >= 0xf)
		set_cpu_cap(c, X86_FEATURE_K8);

	if (cpu_has_xmm2) {
		/* MFENCE stops RDTSC speculation */
		set_cpu_cap(c, X86_FEATURE_MFENCE_RDTSC);
	}

#ifdef CONFIG_X86_64
	if (c->x86 == 0x10) {
		/* do this for boot cpu */
		if (c == &boot_cpu_data)
			check_enable_amd_mmconf_dmi();

		fam10h_check_enable_mmcfg();
	}

	if (c == &boot_cpu_data && c->x86 >= 0xf) {
		unsigned long long tseg;

		/*
		 * Split up direct mapping around the TSEG SMM area.
		 * Don't do it for gbpages because there seems very little
		 * benefit in doing so.
		 */
		if (!rdmsrl_safe(MSR_K8_TSEG_ADDR, &tseg)) {
			unsigned long pfn = tseg >> PAGE_SHIFT;

			printk(KERN_DEBUG "tseg: %010llx\n", tseg);
			if (pfn_range_is_mapped(pfn, pfn + 1))
				set_memory_4k((unsigned long)__va(tseg), 1);
		}
	}
#endif

	/*
	 * Family 0x12 and above processors have APIC timer
	 * running in deep C states.
	 */
	if (c->x86 > 0x11)
		set_cpu_cap(c, X86_FEATURE_ARAT);

	if (c->x86 == 0x10) {
		/*
		 * Disable GART TLB Walk Errors on Fam10h. We do this here
		 * because this is always needed when GART is enabled, even in a
		 * kernel which has no MCE support built in.
		 * BIOS should disable GartTlbWlk Errors themself. If
		 * it doesn't do it here as suggested by the BKDG.
		 *
		 * Fixes: https://bugzilla.kernel.org/show_bug.cgi?id=33012
		 */
		u64 mask;
		int err;

		err = rdmsrl_safe(MSR_AMD64_MCx_MASK(4), &mask);
		if (err == 0) {
			mask |= (1 << 10);
			wrmsrl_safe(MSR_AMD64_MCx_MASK(4), mask);
		}

		/*
		 * On family 10h BIOS may not have properly enabled WC+ support,
		 * causing it to be converted to CD memtype. This may result in
		 * performance degradation for certain nested-paging guests.
		 * Prevent this conversion by clearing bit 24 in
		 * MSR_AMD64_BU_CFG2.
		 *
		 * NOTE: we want to use the _safe accessors so as not to #GP kvm
		 * guests on older kvm hosts.
		 */

		rdmsrl_safe(MSR_AMD64_BU_CFG2, &value);
		value &= ~(1ULL << 24);
		wrmsrl_safe(MSR_AMD64_BU_CFG2, value);

		if (cpu_has_amd_erratum(c, amd_erratum_383))
			set_cpu_bug(c, X86_BUG_AMD_TLB_MMATCH);
	}

	if (cpu_has_amd_erratum(c, amd_erratum_400))
		set_cpu_bug(c, X86_BUG_AMD_APIC_C1E);

	rdmsr_safe(MSR_AMD64_PATCH_LEVEL, &c->microcode, &dummy);
}