static int file_has_perm(const struct cred *cred,
			 struct file *file,
			 u32 av)
{
	struct file_security_struct *fsec = file->f_security;
	struct inode *inode = file_inode(file);
	struct common_audit_data ad;
	u32 sid = cred_sid(cred);
	int rc;

	ad.type = LSM_AUDIT_DATA_FILE;
	ad.u.file = file;

	if (sid != fsec->sid) {
		rc = avc_has_perm(sid, fsec->sid,
				  SECCLASS_FD,
				  FD__USE,
				  &ad);
		if (rc)
			goto out;
	}

	/* av is zero if only checking access to the descriptor. */
	rc = 0;
	if (av)
		rc = inode_has_perm(cred, inode, av, &ad);

out:
	return rc;
}