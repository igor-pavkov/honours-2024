SYSCALL_DEFINE2(fchmod, unsigned int, fd, umode_t, mode)
{
	struct fd f = fdget(fd);
	int err = -EBADF;

	if (f.file) {
		audit_file(f.file);
		err = chmod_common(&f.file->f_path, mode);
		fdput(f);
	}
	return err;
}