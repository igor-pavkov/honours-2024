static inline void flush_unauthorized_files(const struct cred *cred,
					    struct files_struct *files)
{
	struct file *file, *devnull = NULL;
	struct tty_struct *tty;
	int drop_tty = 0;
	unsigned n;

	tty = get_current_tty();
	if (tty) {
		spin_lock(&tty->files_lock);
		if (!list_empty(&tty->tty_files)) {
			struct tty_file_private *file_priv;

			/* Revalidate access to controlling tty.
			   Use file_path_has_perm on the tty path directly
			   rather than using file_has_perm, as this particular
			   open file may belong to another process and we are
			   only interested in the inode-based check here. */
			file_priv = list_first_entry(&tty->tty_files,
						struct tty_file_private, list);
			file = file_priv->file;
			if (file_path_has_perm(cred, file, FILE__READ | FILE__WRITE))
				drop_tty = 1;
		}
		spin_unlock(&tty->files_lock);
		tty_kref_put(tty);
	}
	/* Reset controlling tty. */
	if (drop_tty)
		no_tty();

	/* Revalidate access to inherited open files. */
	n = iterate_fd(files, 0, match_file, cred);
	if (!n) /* none found? */
		return;

	devnull = dentry_open(&selinux_null, O_RDWR, cred);
	if (IS_ERR(devnull))
		devnull = NULL;
	/* replace all the matching ones with this */
	do {
		replace_fd(n - 1, devnull, 0);
	} while ((n = iterate_fd(files, n, match_file, cred)) != 0);
	if (devnull)
		fput(devnull);
}