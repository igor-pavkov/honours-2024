static inline void dequeue_task(struct rq *rq, struct task_struct *p, int flags)
{
	update_rq_clock(rq);
	if (!(flags & DEQUEUE_SAVE))
		sched_info_dequeued(rq, p);
	p->sched_class->dequeue_task(rq, p, flags);
}