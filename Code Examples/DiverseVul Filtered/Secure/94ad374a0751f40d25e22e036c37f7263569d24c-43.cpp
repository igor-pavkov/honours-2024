int file_remove_suid(struct file *file)
{
	struct dentry *dentry = file->f_path.dentry;
	int killsuid = should_remove_suid(dentry);
	int killpriv = security_inode_need_killpriv(dentry);
	int error = 0;

	if (killpriv < 0)
		return killpriv;
	if (killpriv)
		error = security_inode_killpriv(dentry);
	if (!error && killsuid)
		error = __remove_suid(dentry, killsuid);

	return error;
}