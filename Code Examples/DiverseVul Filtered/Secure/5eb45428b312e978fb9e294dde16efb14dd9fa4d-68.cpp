int lxc_unstack_mountpoint(const char *path, bool lazy)
{
	int ret;
	int umounts = 0;

pop_stack:
	ret = umount2(path, lazy ? MNT_DETACH : 0);
	if (ret < 0) {
		/* We consider anything else than EINVAL deadly to prevent going
		 * into an infinite loop. (The other alternative is constantly
		 * parsing /proc/self/mountinfo which is yucky and probably
		 * racy.)
		 */
		if (errno != EINVAL)
			return -errno;
	} else {
		/* Just stop counting when this happens. That'd just be so
		 * stupid that we won't even bother trying to report back the
		 * correct value anymore.
		 */
		if (umounts != INT_MAX)
			umounts++;
		/* We succeeded in umounting. Make sure that there's no other
		 * mountpoint stacked underneath.
		 */
		goto pop_stack;
	}

	return umounts;
}