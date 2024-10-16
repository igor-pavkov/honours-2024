int __mnt_want_write_file(struct file *file)
{
	if (!(file->f_mode & FMODE_WRITER))
		return __mnt_want_write(file->f_path.mnt);
	else
		return mnt_clone_write(file->f_path.mnt);
}