static int rds_notify_cong(struct rds_sock *rs, struct msghdr *msghdr)
{
	uint64_t notify = rs->rs_cong_notify;
	unsigned long flags;
	int err;

	err = put_cmsg(msghdr, SOL_RDS, RDS_CMSG_CONG_UPDATE,
			sizeof(notify), &notify);
	if (err)
		return err;

	spin_lock_irqsave(&rs->rs_lock, flags);
	rs->rs_cong_notify &= ~notify;
	spin_unlock_irqrestore(&rs->rs_lock, flags);

	return 0;
}