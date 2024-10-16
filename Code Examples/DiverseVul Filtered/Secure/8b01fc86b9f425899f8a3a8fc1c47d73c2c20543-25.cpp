void would_dump(struct linux_binprm *bprm, struct file *file)
{
	if (inode_permission(file_inode(file), MAY_READ) < 0)
		bprm->interp_flags |= BINPRM_FLAGS_ENFORCE_NONDUMP;
}