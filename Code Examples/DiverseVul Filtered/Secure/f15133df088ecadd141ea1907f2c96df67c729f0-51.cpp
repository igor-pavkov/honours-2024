static struct file *path_openat(int dfd, struct filename *pathname,
		struct nameidata *nd, const struct open_flags *op, int flags)
{
	struct file *file;
	struct path path;
	int opened = 0;
	int error;

	file = get_empty_filp();
	if (IS_ERR(file))
		return file;

	file->f_flags = op->open_flag;

	if (unlikely(file->f_flags & __O_TMPFILE)) {
		error = do_tmpfile(dfd, pathname, nd, flags, op, file, &opened);
		goto out2;
	}

	error = path_init(dfd, pathname, flags, nd);
	if (unlikely(error))
		goto out;

	error = do_last(nd, &path, file, op, &opened, pathname);
	while (unlikely(error > 0)) { /* trailing symlink */
		struct path link = path;
		void *cookie;
		if (!(nd->flags & LOOKUP_FOLLOW)) {
			path_put_conditional(&path, nd);
			path_put(&nd->path);
			error = -ELOOP;
			break;
		}
		error = may_follow_link(&link, nd);
		if (unlikely(error))
			break;
		nd->flags |= LOOKUP_PARENT;
		nd->flags &= ~(LOOKUP_OPEN|LOOKUP_CREATE|LOOKUP_EXCL);
		error = follow_link(&link, nd, &cookie);
		if (unlikely(error))
			break;
		error = do_last(nd, &path, file, op, &opened, pathname);
		put_link(nd, &link, cookie);
	}
out:
	path_cleanup(nd);
out2:
	if (!(opened & FILE_OPENED)) {
		BUG_ON(!error);
		put_filp(file);
	}
	if (unlikely(error)) {
		if (error == -EOPENSTALE) {
			if (flags & LOOKUP_RCU)
				error = -ECHILD;
			else
				error = -ESTALE;
		}
		file = ERR_PTR(error);
	}
	return file;
}