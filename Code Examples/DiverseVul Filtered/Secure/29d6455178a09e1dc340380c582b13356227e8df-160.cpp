void wake_up_if_idle(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	rcu_read_lock();

	if (!is_idle_task(rcu_dereference(rq->curr)))
		goto out;

	if (set_nr_if_polling(rq->idle)) {
		trace_sched_wake_idle_without_ipi(cpu);
	} else {
		raw_spin_lock_irqsave(&rq->lock, flags);
		if (is_idle_task(rq->curr))
			smp_send_reschedule(cpu);
		/* Else cpu is not in idle, do nothing here */
		raw_spin_unlock_irqrestore(&rq->lock, flags);
	}

out:
	rcu_read_unlock();
}