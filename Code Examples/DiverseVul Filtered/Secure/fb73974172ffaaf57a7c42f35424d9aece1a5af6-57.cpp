static int selinux_inode_alloc_security(struct inode *inode)
{
	struct inode_security_struct *isec = selinux_inode(inode);
	u32 sid = current_sid();

	spin_lock_init(&isec->lock);
	INIT_LIST_HEAD(&isec->list);
	isec->inode = inode;
	isec->sid = SECINITSID_UNLABELED;
	isec->sclass = SECCLASS_FILE;
	isec->task_sid = sid;
	isec->initialized = LABEL_INVALID;

	return 0;
}