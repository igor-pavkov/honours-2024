static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (p->on_rq) {
		struct rq *src_rq, *dst_rq;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		check_preempt_curr(dst_rq, p, 0);
	} else {
		/*
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous cpu our targer instead of where it really is.
		 */
		p->wake_cpu = cpu;
	}
}