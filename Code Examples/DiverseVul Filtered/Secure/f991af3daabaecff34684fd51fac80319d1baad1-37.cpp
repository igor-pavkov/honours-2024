static struct file *do_create(struct ipc_namespace *ipc_ns, struct inode *dir,
			struct path *path, int oflag, umode_t mode,
			struct mq_attr *attr)
{
	const struct cred *cred = current_cred();
	int ret;

	if (attr) {
		ret = mq_attr_ok(ipc_ns, attr);
		if (ret)
			return ERR_PTR(ret);
		/* store for use during create */
		path->dentry->d_fsdata = attr;
	} else {
		struct mq_attr def_attr;

		def_attr.mq_maxmsg = min(ipc_ns->mq_msg_max,
					 ipc_ns->mq_msg_default);
		def_attr.mq_msgsize = min(ipc_ns->mq_msgsize_max,
					  ipc_ns->mq_msgsize_default);
		ret = mq_attr_ok(ipc_ns, &def_attr);
		if (ret)
			return ERR_PTR(ret);
	}

	mode &= ~current_umask();
	ret = vfs_create(dir, path->dentry, mode, true);
	path->dentry->d_fsdata = NULL;
	if (ret)
		return ERR_PTR(ret);
	return dentry_open(path, oflag, cred);
}