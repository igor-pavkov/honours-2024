static void selinux_file_set_fowner(struct file *file)
{
	struct file_security_struct *fsec;

	fsec = selinux_file(file);
	fsec->fown_sid = current_sid();
}