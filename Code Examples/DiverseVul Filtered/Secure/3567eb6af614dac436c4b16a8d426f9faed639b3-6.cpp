void snd_seq_check_queue(struct snd_seq_queue *q, int atomic, int hop)
{
	unsigned long flags;
	struct snd_seq_event_cell *cell;

	if (q == NULL)
		return;

	/* make this function non-reentrant */
	spin_lock_irqsave(&q->check_lock, flags);
	if (q->check_blocked) {
		q->check_again = 1;
		spin_unlock_irqrestore(&q->check_lock, flags);
		return;		/* other thread is already checking queues */
	}
	q->check_blocked = 1;
	spin_unlock_irqrestore(&q->check_lock, flags);

      __again:
	/* Process tick queue... */
	while ((cell = snd_seq_prioq_cell_peek(q->tickq)) != NULL) {
		if (snd_seq_compare_tick_time(&q->timer->tick.cur_tick,
					      &cell->event.time.tick)) {
			cell = snd_seq_prioq_cell_out(q->tickq);
			if (cell)
				snd_seq_dispatch_event(cell, atomic, hop);
		} else {
			/* event remains in the queue */
			break;
		}
	}


	/* Process time queue... */
	while ((cell = snd_seq_prioq_cell_peek(q->timeq)) != NULL) {
		if (snd_seq_compare_real_time(&q->timer->cur_time,
					      &cell->event.time.time)) {
			cell = snd_seq_prioq_cell_out(q->timeq);
			if (cell)
				snd_seq_dispatch_event(cell, atomic, hop);
		} else {
			/* event remains in the queue */
			break;
		}
	}

	/* free lock */
	spin_lock_irqsave(&q->check_lock, flags);
	if (q->check_again) {
		q->check_again = 0;
		spin_unlock_irqrestore(&q->check_lock, flags);
		goto __again;
	}
	q->check_blocked = 0;
	spin_unlock_irqrestore(&q->check_lock, flags);
}