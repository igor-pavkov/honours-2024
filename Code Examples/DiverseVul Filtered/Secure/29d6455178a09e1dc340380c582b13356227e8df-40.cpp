static struct rq *move_queued_task(struct rq *rq, struct task_struct *p, int new_cpu)
{
	lockdep_assert_held(&rq->lock);

	p->on_rq = TASK_ON_RQ_MIGRATING;
	dequeue_task(rq, p, 0);
	set_task_cpu(p, new_cpu);
	raw_spin_unlock(&rq->lock);

	rq = cpu_rq(new_cpu);

	raw_spin_lock(&rq->lock);
	BUG_ON(task_cpu(p) != new_cpu);
	enqueue_task(rq, p, 0);
	p->on_rq = TASK_ON_RQ_QUEUED;
	check_preempt_curr(rq, p, 0);

	return rq;
}