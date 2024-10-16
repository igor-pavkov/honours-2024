static int __compat_get_timespec64(struct timespec64 *ts64,
				   const struct compat_timespec __user *cts)
{
	struct compat_timespec ts;
	int ret;

	ret = copy_from_user(&ts, cts, sizeof(ts));
	if (ret)
		return -EFAULT;

	ts64->tv_sec = ts.tv_sec;
	ts64->tv_nsec = ts.tv_nsec;

	return 0;
}