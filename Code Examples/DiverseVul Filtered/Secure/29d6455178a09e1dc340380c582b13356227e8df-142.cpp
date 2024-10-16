void update_rq_clock(struct rq *rq)
{
	s64 delta;

	lockdep_assert_held(&rq->lock);

	if (rq->clock_skip_update & RQCF_ACT_SKIP)
		return;

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	if (delta < 0)
		return;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}