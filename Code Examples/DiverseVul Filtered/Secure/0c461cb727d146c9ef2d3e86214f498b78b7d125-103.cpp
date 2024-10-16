static int selinux_kernel_module_from_file(struct file *file)
{
	struct common_audit_data ad;
	struct inode_security_struct *isec;
	struct file_security_struct *fsec;
	u32 sid = current_sid();
	int rc;

	/* init_module */
	if (file == NULL)
		return avc_has_perm(sid, sid, SECCLASS_SYSTEM,
					SYSTEM__MODULE_LOAD, NULL);

	/* finit_module */

	ad.type = LSM_AUDIT_DATA_FILE;
	ad.u.file = file;

	fsec = file->f_security;
	if (sid != fsec->sid) {
		rc = avc_has_perm(sid, fsec->sid, SECCLASS_FD, FD__USE, &ad);
		if (rc)
			return rc;
	}

	isec = inode_security(file_inode(file));
	return avc_has_perm(sid, isec->sid, SECCLASS_SYSTEM,
				SYSTEM__MODULE_LOAD, &ad);
}