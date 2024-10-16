static long smb3_punch_hole(struct file *file, struct cifs_tcon *tcon,
			    loff_t offset, loff_t len)
{
	struct inode *inode;
	struct cifsFileInfo *cfile = file->private_data;
	struct file_zero_data_information fsctl_buf;
	long rc;
	unsigned int xid;
	__u8 set_sparse = 1;

	xid = get_xid();

	inode = d_inode(cfile->dentry);

	/* Need to make file sparse, if not already, before freeing range. */
	/* Consider adding equivalent for compressed since it could also work */
	if (!smb2_set_sparse(xid, tcon, cfile, inode, set_sparse)) {
		rc = -EOPNOTSUPP;
		free_xid(xid);
		return rc;
	}

	filemap_invalidate_lock(inode->i_mapping);
	/*
	 * We implement the punch hole through ioctl, so we need remove the page
	 * caches first, otherwise the data may be inconsistent with the server.
	 */
	truncate_pagecache_range(inode, offset, offset + len - 1);

	cifs_dbg(FYI, "Offset %lld len %lld\n", offset, len);

	fsctl_buf.FileOffset = cpu_to_le64(offset);
	fsctl_buf.BeyondFinalZero = cpu_to_le64(offset + len);

	rc = SMB2_ioctl(xid, tcon, cfile->fid.persistent_fid,
			cfile->fid.volatile_fid, FSCTL_SET_ZERO_DATA,
			true /* is_fctl */, (char *)&fsctl_buf,
			sizeof(struct file_zero_data_information),
			CIFSMaxBufSize, NULL, NULL);
	free_xid(xid);
	filemap_invalidate_unlock(inode->i_mapping);
	return rc;
}