static int sock_set_timeout(long *timeo_p, char __user *optval, int optlen)
{
	struct timeval tv;

	if (optlen < sizeof(tv))
		return -EINVAL;
	if (copy_from_user(&tv, optval, sizeof(tv)))
		return -EFAULT;
	if (tv.tv_usec < 0 || tv.tv_usec >= USEC_PER_SEC)
		return -EDOM;

	if (tv.tv_sec < 0) {
		static int warned __read_mostly;

		*timeo_p = 0;
		if (warned < 10 && net_ratelimit()) {
			warned++;
			printk(KERN_INFO "sock_set_timeout: `%s' (pid %d) "
			       "tries to set negative timeout\n",
				current->comm, task_pid_nr(current));
		}
		return 0;
	}
	*timeo_p = MAX_SCHEDULE_TIMEOUT;
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
		return 0;
	if (tv.tv_sec < (MAX_SCHEDULE_TIMEOUT/HZ - 1))
		*timeo_p = tv.tv_sec*HZ + (tv.tv_usec+(1000000/HZ-1))/(1000000/HZ);
	return 0;
}