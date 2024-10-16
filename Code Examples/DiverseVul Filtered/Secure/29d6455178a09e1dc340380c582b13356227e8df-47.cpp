void resched_cpu(int cpu)
{
	struct rq *rq = cpu_rq(cpu);
	unsigned long flags;

	if (!raw_spin_trylock_irqsave(&rq->lock, flags))
		return;
	resched_curr(rq);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}