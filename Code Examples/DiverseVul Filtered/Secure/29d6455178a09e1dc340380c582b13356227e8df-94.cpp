static void ttwu_queue_remote(struct task_struct *p, int cpu)
{
	struct rq *rq = cpu_rq(cpu);

	if (llist_add(&p->wake_entry, &cpu_rq(cpu)->wake_list)) {
		if (!set_nr_if_polling(rq->idle))
			smp_send_reschedule(cpu);
		else
			trace_sched_wake_idle_without_ipi(cpu);
	}
}