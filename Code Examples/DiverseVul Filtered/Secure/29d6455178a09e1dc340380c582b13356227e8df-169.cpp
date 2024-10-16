bool cpus_share_cache(int this_cpu, int that_cpu)
{
	return per_cpu(sd_llc_id, this_cpu) == per_cpu(sd_llc_id, that_cpu);
}