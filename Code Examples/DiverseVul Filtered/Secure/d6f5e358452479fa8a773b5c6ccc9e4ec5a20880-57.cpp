static long smb3_zero_range(struct file *file, struct cifs_tcon *tcon,
			    loff_t offset, loff_t len, bool keep_size)
{
	struct cifs_ses *ses = tcon->ses;
	struct inode *inode;
	struct cifsInodeInfo *cifsi;
	struct cifsFileInfo *cfile = file->private_data;
	struct file_zero_data_information fsctl_buf;
	long rc;
	unsigned int xid;
	__le64 eof;

	xid = get_xid();

	inode = d_inode(cfile->dentry);
	cifsi = CIFS_I(inode);

	trace_smb3_zero_enter(xid, cfile->fid.persistent_fid, tcon->tid,
			      ses->Suid, offset, len);

	/*
	 * We zero the range through ioctl, so we need remove the page caches
	 * first, otherwise the data may be inconsistent with the server.
	 */
	truncate_pagecache_range(inode, offset, offset + len - 1);

	/* if file not oplocked can't be sure whether asking to extend size */
	if (!CIFS_CACHE_READ(cifsi))
		if (keep_size == false) {
			rc = -EOPNOTSUPP;
			trace_smb3_zero_err(xid, cfile->fid.persistent_fid,
				tcon->tid, ses->Suid, offset, len, rc);
			free_xid(xid);
			return rc;
		}

	cifs_dbg(FYI, "Offset %lld len %lld\n", offset, len);

	fsctl_buf.FileOffset = cpu_to_le64(offset);
	fsctl_buf.BeyondFinalZero = cpu_to_le64(offset + len);

	rc = SMB2_ioctl(xid, tcon, cfile->fid.persistent_fid,
			cfile->fid.volatile_fid, FSCTL_SET_ZERO_DATA, true,
			(char *)&fsctl_buf,
			sizeof(struct file_zero_data_information),
			0, NULL, NULL);
	if (rc)
		goto zero_range_exit;

	/*
	 * do we also need to change the size of the file?
	 */
	if (keep_size == false && i_size_read(inode) < offset + len) {
		eof = cpu_to_le64(offset + len);
		rc = SMB2_set_eof(xid, tcon, cfile->fid.persistent_fid,
				  cfile->fid.volatile_fid, cfile->pid, &eof);
	}

 zero_range_exit:
	free_xid(xid);
	if (rc)
		trace_smb3_zero_err(xid, cfile->fid.persistent_fid, tcon->tid,
			      ses->Suid, offset, len, rc);
	else
		trace_smb3_zero_done(xid, cfile->fid.persistent_fid, tcon->tid,
			      ses->Suid, offset, len);
	return rc;
}