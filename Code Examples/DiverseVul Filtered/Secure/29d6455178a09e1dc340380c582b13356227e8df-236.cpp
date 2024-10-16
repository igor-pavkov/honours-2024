void sched_setnuma(struct task_struct *p, int nid)
{
	struct rq *rq;
	unsigned long flags;
	bool queued, running;

	rq = task_rq_lock(p, &flags);
	queued = task_on_rq_queued(p);
	running = task_current(rq, p);

	if (queued)
		dequeue_task(rq, p, DEQUEUE_SAVE);
	if (running)
		put_prev_task(rq, p);

	p->numa_preferred_nid = nid;

	if (running)
		p->sched_class->set_curr_task(rq);
	if (queued)
		enqueue_task(rq, p, ENQUEUE_RESTORE);
	task_rq_unlock(rq, p, &flags);
}