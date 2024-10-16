static int selinux_binder_transfer_file(struct task_struct *from,
					struct task_struct *to,
					struct file *file)
{
	u32 sid = task_sid(to);
	struct file_security_struct *fsec = file->f_security;
	struct dentry *dentry = file->f_path.dentry;
	struct inode_security_struct *isec;
	struct common_audit_data ad;
	int rc;

	ad.type = LSM_AUDIT_DATA_PATH;
	ad.u.path = file->f_path;

	if (sid != fsec->sid) {
		rc = avc_has_perm(sid, fsec->sid,
				  SECCLASS_FD,
				  FD__USE,
				  &ad);
		if (rc)
			return rc;
	}

	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;

	isec = backing_inode_security(dentry);
	return avc_has_perm(sid, isec->sid, isec->sclass, file_to_av(file),
			    &ad);
}