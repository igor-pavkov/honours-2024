static int proc_gid_map_open(struct inode *inode, struct file *file)
{
	return proc_id_map_open(inode, file, &proc_gid_seq_operations);
}