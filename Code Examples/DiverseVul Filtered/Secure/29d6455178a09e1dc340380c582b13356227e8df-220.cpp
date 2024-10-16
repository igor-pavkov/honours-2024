void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
{
	struct rq *rq = task_rq(p);
	bool queued, running;

	lockdep_assert_held(&p->pi_lock);

	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued) {
		/*
		 * Because __kthread_bind() calls this on blocked tasks without
		 * holding rq->lock.
		 */
		lockdep_assert_held(&rq->lock);
		dequeue_task(rq, p, DEQUEUE_SAVE);
	}
	if (running)
		put_prev_task(rq, p);

	p->sched_class->set_cpus_allowed(p, new_mask);

	if (running)
		p->sched_class->set_curr_task(rq);
	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE);
}