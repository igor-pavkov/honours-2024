static int ip_vs_stats_percpu_seq_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, ip_vs_stats_percpu_show);
}