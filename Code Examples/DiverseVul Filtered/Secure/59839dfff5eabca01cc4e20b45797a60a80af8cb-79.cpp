static void kvm_set_time_scale(uint32_t tsc_khz, struct pvclock_vcpu_time_info *hv_clock)
{
	uint64_t nsecs = 1000000000LL;
	int32_t  shift = 0;
	uint64_t tps64;
	uint32_t tps32;

	tps64 = tsc_khz * 1000LL;
	while (tps64 > nsecs*2) {
		tps64 >>= 1;
		shift--;
	}

	tps32 = (uint32_t)tps64;
	while (tps32 <= (uint32_t)nsecs) {
		tps32 <<= 1;
		shift++;
	}

	hv_clock->tsc_shift = shift;
	hv_clock->tsc_to_system_mul = div_frac(nsecs, tps32);

	pr_debug("%s: tsc_khz %u, tsc_shift %d, tsc_mul %u\n",
		 __func__, tsc_khz, hv_clock->tsc_shift,
		 hv_clock->tsc_to_system_mul);
}