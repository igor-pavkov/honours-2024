static void __hrtick_start(void *arg)
{
	struct rq *rq = arg;

	raw_spin_lock(&rq->lock);
	__hrtick_restart(rq);
	rq->hrtick_csd_pending = 0;
	raw_spin_unlock(&rq->lock);
}