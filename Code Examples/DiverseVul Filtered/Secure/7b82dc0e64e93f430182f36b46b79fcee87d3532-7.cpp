static struct file *__dentry_open(struct dentry *dentry, struct vfsmount *mnt,
					int flags, struct file *f,
					int (*open)(struct inode *, struct file *))
{
	struct inode *inode;
	int error;

	f->f_flags = flags;
	f->f_mode = ((flags+1) & O_ACCMODE) | FMODE_LSEEK |
				FMODE_PREAD | FMODE_PWRITE;
	inode = dentry->d_inode;
	if (f->f_mode & FMODE_WRITE) {
		error = get_write_access(inode);
		if (error)
			goto cleanup_file;
	}

	f->f_mapping = inode->i_mapping;
	f->f_path.dentry = dentry;
	f->f_path.mnt = mnt;
	f->f_pos = 0;
	f->f_op = fops_get(inode->i_fop);
	file_move(f, &inode->i_sb->s_files);

	if (!open && f->f_op)
		open = f->f_op->open;
	if (open) {
		error = open(inode, f);
		if (error)
			goto cleanup_all;
	}

	f->f_flags &= ~(O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC);

	file_ra_state_init(&f->f_ra, f->f_mapping->host->i_mapping);

	/* NB: we're sure to have correct a_ops only after f_op->open */
	if (f->f_flags & O_DIRECT) {
		if (!f->f_mapping->a_ops ||
		    ((!f->f_mapping->a_ops->direct_IO) &&
		    (!f->f_mapping->a_ops->get_xip_page))) {
			fput(f);
			f = ERR_PTR(-EINVAL);
		}
	}

	return f;

cleanup_all:
	fops_put(f->f_op);
	if (f->f_mode & FMODE_WRITE)
		put_write_access(inode);
	file_kill(f);
	f->f_path.dentry = NULL;
	f->f_path.mnt = NULL;
cleanup_file:
	put_filp(f);
	dput(dentry);
	mntput(mnt);
	return ERR_PTR(error);
}