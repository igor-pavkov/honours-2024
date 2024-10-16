COMPAT_SYSCALL_DEFINE3(rt_sigqueueinfo,
			compat_pid_t, pid,
			int, sig,
			struct compat_siginfo __user *, uinfo)
{
	siginfo_t info;
	int ret = copy_siginfo_from_user32(&info, uinfo);
	if (unlikely(ret))
		return ret;
	return do_rt_sigqueueinfo(pid, sig, &info);
}