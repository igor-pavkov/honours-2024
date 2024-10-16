struct pipe_inode_info *get_pipe_info(struct file *file, bool for_splice)
{
	struct pipe_inode_info *pipe = file->private_data;

	if (file->f_op != &pipefifo_fops || !pipe)
		return NULL;
#ifdef CONFIG_WATCH_QUEUE
	if (for_splice && pipe->watch_queue)
		return NULL;
#endif
	return pipe;
}