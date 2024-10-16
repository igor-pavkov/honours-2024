static int clusterip_proc_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &clusterip_seq_ops);

	if (!ret) {
		struct seq_file *sf = file->private_data;
		struct clusterip_config *c = PDE(inode)->data;

		sf->private = c;

		clusterip_config_get(c);
	}

	return ret;
}