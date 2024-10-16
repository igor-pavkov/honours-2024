u16 amd_get_nb_id(int cpu)
{
	u16 id = 0;
#ifdef CONFIG_SMP
	id = per_cpu(cpu_llc_id, cpu);
#endif
	return id;
}