int sys_waitpid(pid_t pid,int *status,int options)
{
#ifdef HAVE_WAITPID
	return waitpid(pid,status,options);
#else /* HAVE_WAITPID */
	return wait4(pid, status, options, NULL);
#endif /* HAVE_WAITPID */
}