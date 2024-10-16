int rds_notify_queue_get(struct rds_sock *rs, struct msghdr *msghdr)
{
	struct rds_notifier *notifier;
	struct rds_rdma_notify cmsg = { 0 }; /* fill holes with zero */
	unsigned int count = 0, max_messages = ~0U;
	unsigned long flags;
	LIST_HEAD(copy);
	int err = 0;


	/* put_cmsg copies to user space and thus may sleep. We can't do this
	 * with rs_lock held, so first grab as many notifications as we can stuff
	 * in the user provided cmsg buffer. We don't try to copy more, to avoid
	 * losing notifications - except when the buffer is so small that it wouldn't
	 * even hold a single notification. Then we give him as much of this single
	 * msg as we can squeeze in, and set MSG_CTRUNC.
	 */
	if (msghdr) {
		max_messages = msghdr->msg_controllen / CMSG_SPACE(sizeof(cmsg));
		if (!max_messages)
			max_messages = 1;
	}

	spin_lock_irqsave(&rs->rs_lock, flags);
	while (!list_empty(&rs->rs_notify_queue) && count < max_messages) {
		notifier = list_entry(rs->rs_notify_queue.next,
				struct rds_notifier, n_list);
		list_move(&notifier->n_list, &copy);
		count++;
	}
	spin_unlock_irqrestore(&rs->rs_lock, flags);

	if (!count)
		return 0;

	while (!list_empty(&copy)) {
		notifier = list_entry(copy.next, struct rds_notifier, n_list);

		if (msghdr) {
			cmsg.user_token = notifier->n_user_token;
			cmsg.status = notifier->n_status;

			err = put_cmsg(msghdr, SOL_RDS, RDS_CMSG_RDMA_STATUS,
				       sizeof(cmsg), &cmsg);
			if (err)
				break;
		}

		list_del_init(&notifier->n_list);
		kfree(notifier);
	}

	/* If we bailed out because of an error in put_cmsg,
	 * we may be left with one or more notifications that we
	 * didn't process. Return them to the head of the list. */
	if (!list_empty(&copy)) {
		spin_lock_irqsave(&rs->rs_lock, flags);
		list_splice(&copy, &rs->rs_notify_queue);
		spin_unlock_irqrestore(&rs->rs_lock, flags);
	}

	return err;
}