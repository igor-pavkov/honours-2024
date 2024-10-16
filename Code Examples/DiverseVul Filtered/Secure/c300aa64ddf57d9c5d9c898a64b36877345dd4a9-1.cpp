static inline u64 vgettsc(cycle_t *cycle_now)
{
	long v;
	struct pvclock_gtod_data *gtod = &pvclock_gtod_data;

	*cycle_now = read_tsc();

	v = (*cycle_now - gtod->clock.cycle_last) & gtod->clock.mask;
	return v * gtod->clock.mult;
}