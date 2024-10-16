static int file_map_prot_check(struct file *file, unsigned long prot, int shared)
{
	const struct cred *cred = current_cred();
	u32 sid = cred_sid(cred);
	int rc = 0;

	if (default_noexec &&
	    (prot & PROT_EXEC) && (!file || IS_PRIVATE(file_inode(file)) ||
				   (!shared && (prot & PROT_WRITE)))) {
		/*
		 * We are making executable an anonymous mapping or a
		 * private file mapping that will also be writable.
		 * This has an additional check.
		 */
		rc = avc_has_perm(&selinux_state,
				  sid, sid, SECCLASS_PROCESS,
				  PROCESS__EXECMEM, NULL);
		if (rc)
			goto error;
	}

	if (file) {
		/* read access is always possible with a mapping */
		u32 av = FILE__READ;

		/* write access only matters if the mapping is shared */
		if (shared && (prot & PROT_WRITE))
			av |= FILE__WRITE;

		if (prot & PROT_EXEC)
			av |= FILE__EXECUTE;

		return file_has_perm(cred, file, av);
	}

error:
	return rc;
}