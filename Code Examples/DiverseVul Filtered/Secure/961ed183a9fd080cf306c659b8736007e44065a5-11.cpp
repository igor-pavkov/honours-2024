static int clusterip_proc_release(struct inode *inode, struct file *file)
{
	struct clusterip_config *c = PDE(inode)->data;
	int ret;

	ret = seq_release(inode, file);

	if (!ret)
		clusterip_config_put(c);

	return ret;
}