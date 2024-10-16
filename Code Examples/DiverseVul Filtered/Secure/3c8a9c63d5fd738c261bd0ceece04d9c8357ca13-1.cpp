static int tun_get_iff(struct net *net, struct file *file, struct ifreq *ifr)
{
	struct tun_struct *tun = tun_get(file);

	if (!tun)
		return -EBADFD;

	DBG(KERN_INFO "%s: tun_get_iff\n", tun->dev->name);

	strcpy(ifr->ifr_name, tun->dev->name);

	ifr->ifr_flags = tun_flags(tun);

	tun_put(tun);
	return 0;
}