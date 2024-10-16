static inline void update_t_max_wait(transaction_t *transaction,
				     unsigned long ts)
{
#ifdef CONFIG_JBD2_DEBUG
	if (jbd2_journal_enable_debug &&
	    time_after(transaction->t_start, ts)) {
		ts = jbd2_time_diff(ts, transaction->t_start);
		spin_lock(&transaction->t_handle_lock);
		if (ts > transaction->t_max_wait)
			transaction->t_max_wait = ts;
		spin_unlock(&transaction->t_handle_lock);
	}
#endif
}