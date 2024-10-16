static int ioctl_has_perm(const struct cred *cred, struct file *file,
		u32 requested, u16 cmd)
{
	struct common_audit_data ad;
	struct file_security_struct *fsec = file->f_security;
	struct inode *inode = file_inode(file);
	struct inode_security_struct *isec;
	struct lsm_ioctlop_audit ioctl;
	u32 ssid = cred_sid(cred);
	int rc;
	u8 driver = cmd >> 8;
	u8 xperm = cmd & 0xff;

	ad.type = LSM_AUDIT_DATA_IOCTL_OP;
	ad.u.op = &ioctl;
	ad.u.op->cmd = cmd;
	ad.u.op->path = file->f_path;

	if (ssid != fsec->sid) {
		rc = avc_has_perm(ssid, fsec->sid,
				SECCLASS_FD,
				FD__USE,
				&ad);
		if (rc)
			goto out;
	}

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	isec = inode_security(inode);
	rc = avc_has_extended_perms(ssid, isec->sid, isec->sclass,
			requested, driver, xperm, &ad);
out:
	return rc;
}