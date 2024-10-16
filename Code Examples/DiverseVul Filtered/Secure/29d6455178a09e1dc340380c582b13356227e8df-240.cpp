static struct rq *__migrate_task(struct rq *rq, struct task_struct *p, int dest_cpu)
{
	if (unlikely(!cpu_active(dest_cpu)))
		return rq;

	/* Affinity changed (again). */
	if (!cpumask_test_cpu(dest_cpu, tsk_cpus_allowed(p)))
		return rq;

	rq = move_queued_task(rq, p, dest_cpu);

	return rq;
}