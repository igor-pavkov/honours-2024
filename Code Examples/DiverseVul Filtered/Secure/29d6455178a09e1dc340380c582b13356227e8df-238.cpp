static inline void balance_callback(struct rq *rq)
{
	if (unlikely(rq->balance_callback))
		__balance_callback(rq);
}