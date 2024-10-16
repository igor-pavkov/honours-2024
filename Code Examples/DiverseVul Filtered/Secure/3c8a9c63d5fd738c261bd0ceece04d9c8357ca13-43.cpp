static int tun_chr_fasync(int fd, struct file *file, int on)
{
	struct tun_struct *tun = tun_get(file);
	int ret;

	if (!tun)
		return -EBADFD;

	DBG(KERN_INFO "%s: tun_chr_fasync %d\n", tun->dev->name, on);

	lock_kernel();
	if ((ret = fasync_helper(fd, file, on, &tun->fasync)) < 0)
		goto out;

	if (on) {
		ret = __f_setown(file, task_pid(current), PIDTYPE_PID, 0);
		if (ret)
			goto out;
		tun->flags |= TUN_FASYNC;
	} else
		tun->flags &= ~TUN_FASYNC;
	ret = 0;
out:
	unlock_kernel();
	tun_put(tun);
	return ret;
}