int put_compat_itimerspec64(const struct itimerspec64 *its,
			struct compat_itimerspec __user *uits)
{
	if (__compat_put_timespec64(&its->it_interval, &uits->it_interval) ||
	    __compat_put_timespec64(&its->it_value, &uits->it_value))
		return -EFAULT;
	return 0;
}