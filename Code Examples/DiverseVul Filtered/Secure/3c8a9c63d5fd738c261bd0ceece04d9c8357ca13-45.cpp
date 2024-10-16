static int tun_chr_open(struct inode *inode, struct file * file)
{
	struct tun_file *tfile;
	cycle_kernel_lock();
	DBG1(KERN_INFO "tunX: tun_chr_open\n");

	tfile = kmalloc(sizeof(*tfile), GFP_KERNEL);
	if (!tfile)
		return -ENOMEM;
	atomic_set(&tfile->count, 0);
	tfile->tun = NULL;
	tfile->net = get_net(current->nsproxy->net_ns);
	file->private_data = tfile;
	return 0;
}