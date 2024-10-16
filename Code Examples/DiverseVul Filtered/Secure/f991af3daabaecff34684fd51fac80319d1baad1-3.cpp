static int mqueue_flush_file(struct file *filp, fl_owner_t id)
{
	struct mqueue_inode_info *info = MQUEUE_I(file_inode(filp));

	spin_lock(&info->lock);
	if (task_tgid(current) == info->notify_owner)
		remove_notification(info);

	spin_unlock(&info->lock);
	return 0;
}