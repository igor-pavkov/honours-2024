static void set_cpu_rq_start_time(void)
{
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);
	rq->age_stamp = sched_clock_cpu(cpu);
}