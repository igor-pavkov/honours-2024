static void get_counters(const struct ebt_counter *oldcounters,
   struct ebt_counter *counters, unsigned int nentries)
{
	int i, cpu;
	struct ebt_counter *counter_base;

	/* counters of cpu 0 */
	memcpy(counters, oldcounters,
	       sizeof(struct ebt_counter) * nentries);

	/* add other counters to those of cpu 0 */
	for_each_possible_cpu(cpu) {
		if (cpu == 0)
			continue;
		counter_base = COUNTER_BASE(oldcounters, nentries, cpu);
		for (i = 0; i < nentries; i++) {
			counters[i].pcnt += counter_base[i].pcnt;
			counters[i].bcnt += counter_base[i].bcnt;
		}
	}
}