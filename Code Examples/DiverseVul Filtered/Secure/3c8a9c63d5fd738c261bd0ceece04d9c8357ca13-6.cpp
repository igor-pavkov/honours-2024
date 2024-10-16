static int tun_attach(struct tun_struct *tun, struct file *file)
{
	struct tun_file *tfile = file->private_data;
	const struct cred *cred = current_cred();
	int err;

	ASSERT_RTNL();

	/* Check permissions */
	if (((tun->owner != -1 && cred->euid != tun->owner) ||
	     (tun->group != -1 && !in_egroup_p(tun->group))) &&
		!capable(CAP_NET_ADMIN))
		return -EPERM;

	netif_tx_lock_bh(tun->dev);

	err = -EINVAL;
	if (tfile->tun)
		goto out;

	err = -EBUSY;
	if (tun->tfile)
		goto out;

	err = 0;
	tfile->tun = tun;
	tun->tfile = tfile;
	dev_hold(tun->dev);
	sock_hold(tun->sk);
	atomic_inc(&tfile->count);

out:
	netif_tx_unlock_bh(tun->dev);
	return err;
}