static void __balance_callback(struct rq *rq)
{
	struct callback_head *head, *next;
	void (*func)(struct rq *rq);
	unsigned long flags;

	raw_spin_lock_irqsave(&rq->lock, flags);
	head = rq->balance_callback;
	rq->balance_callback = NULL;
	while (head) {
		func = (void (*)(struct rq *))head->func;
		next = head->next;
		head->next = NULL;
		head = next;

		func(rq);
	}
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}