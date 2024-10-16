static int rds_next_incoming(struct rds_sock *rs, struct rds_incoming **inc)
{
	unsigned long flags;

	if (!*inc) {
		read_lock_irqsave(&rs->rs_recv_lock, flags);
		if (!list_empty(&rs->rs_recv_queue)) {
			*inc = list_entry(rs->rs_recv_queue.next,
					  struct rds_incoming,
					  i_item);
			rds_inc_addref(*inc);
		}
		read_unlock_irqrestore(&rs->rs_recv_lock, flags);
	}

	return *inc != NULL;
}