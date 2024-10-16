void sched_avg_update(struct rq *rq)
{
	s64 period = sched_avg_period();

	while ((s64)(rq_clock(rq) - rq->age_stamp) > period) {
		/*
		 * Inline assembly required to prevent the compiler
		 * optimising this loop into a divmod call.
		 * See __iter_div_u64_rem() for another example of this.
		 */
		asm("" : "+rm" (rq->age_stamp));
		rq->age_stamp += period;
		rq->rt_avg /= 2;
	}
}