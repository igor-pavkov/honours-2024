int tg_set_cfs_quota(struct task_group *tg, long cfs_quota_us)
{
	u64 quota, period;

	period = ktime_to_ns(tg->cfs_bandwidth.period);
	if (cfs_quota_us < 0)
		quota = RUNTIME_INF;
	else
		quota = (u64)cfs_quota_us * NSEC_PER_USEC;

	return tg_set_cfs_bandwidth(tg, period, quota);
}