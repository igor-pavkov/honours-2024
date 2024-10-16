static int apparmor_file_mmap(struct file *file, unsigned long reqprot,
			      unsigned long prot, unsigned long flags,
			      unsigned long addr, unsigned long addr_only)
{
	int rc = 0;

	/* do DAC check */
	rc = cap_file_mmap(file, reqprot, prot, flags, addr, addr_only);
	if (rc || addr_only)
		return rc;

	return common_mmap(OP_FMMAP, file, prot, flags);
}