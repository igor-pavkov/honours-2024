static bool cpu_has_amd_erratum(struct cpuinfo_x86 *cpu, const int *erratum)
{
	int osvw_id = *erratum++;
	u32 range;
	u32 ms;

	if (osvw_id >= 0 && osvw_id < 65536 &&
	    cpu_has(cpu, X86_FEATURE_OSVW)) {
		u64 osvw_len;

		rdmsrl(MSR_AMD64_OSVW_ID_LENGTH, osvw_len);
		if (osvw_id < osvw_len) {
			u64 osvw_bits;

			rdmsrl(MSR_AMD64_OSVW_STATUS + (osvw_id >> 6),
			    osvw_bits);
			return osvw_bits & (1ULL << (osvw_id & 0x3f));
		}
	}

	/* OSVW unavailable or ID unknown, match family-model-stepping range */
	ms = (cpu->x86_model << 4) | cpu->x86_mask;
	while ((range = *erratum++))
		if ((cpu->x86 == AMD_MODEL_RANGE_FAMILY(range)) &&
		    (ms >= AMD_MODEL_RANGE_START(range)) &&
		    (ms <= AMD_MODEL_RANGE_END(range)))
			return true;

	return false;
}