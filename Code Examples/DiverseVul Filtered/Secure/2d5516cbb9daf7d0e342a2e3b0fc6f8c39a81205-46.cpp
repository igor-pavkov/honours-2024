static int unshare_thread(unsigned long unshare_flags)
{
	if (unshare_flags & CLONE_THREAD)
		return -EINVAL;

	return 0;
}