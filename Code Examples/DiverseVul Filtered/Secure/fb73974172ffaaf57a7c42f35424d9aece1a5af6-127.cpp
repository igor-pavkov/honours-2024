static int selinux_file_alloc_security(struct file *file)
{
	struct file_security_struct *fsec = selinux_file(file);
	u32 sid = current_sid();

	fsec->sid = sid;
	fsec->fown_sid = sid;

	return 0;
}