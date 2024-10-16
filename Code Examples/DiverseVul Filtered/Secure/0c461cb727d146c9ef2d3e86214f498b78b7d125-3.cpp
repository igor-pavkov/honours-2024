static int file_alloc_security(struct file *file)
{
	struct file_security_struct *fsec;
	u32 sid = current_sid();

	fsec = kmem_cache_zalloc(file_security_cache, GFP_KERNEL);
	if (!fsec)
		return -ENOMEM;

	fsec->sid = sid;
	fsec->fown_sid = sid;
	file->f_security = fsec;

	return 0;
}