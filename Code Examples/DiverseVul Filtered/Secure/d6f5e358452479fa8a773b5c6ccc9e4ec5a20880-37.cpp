static long smb3_collapse_range(struct file *file, struct cifs_tcon *tcon,
			    loff_t off, loff_t len)
{
	int rc;
	unsigned int xid;
	struct inode *inode;
	struct cifsFileInfo *cfile = file->private_data;
	struct cifsInodeInfo *cifsi;
	__le64 eof;

	xid = get_xid();

	inode = d_inode(cfile->dentry);
	cifsi = CIFS_I(inode);

	if (off >= i_size_read(inode) ||
	    off + len >= i_size_read(inode)) {
		rc = -EINVAL;
		goto out;
	}

	rc = smb2_copychunk_range(xid, cfile, cfile, off + len,
				  i_size_read(inode) - off - len, off);
	if (rc < 0)
		goto out;

	eof = cpu_to_le64(i_size_read(inode) - len);
	rc = SMB2_set_eof(xid, tcon, cfile->fid.persistent_fid,
			  cfile->fid.volatile_fid, cfile->pid, &eof);
	if (rc < 0)
		goto out;

	rc = 0;

	cifsi->server_eof = i_size_read(inode) - len;
	truncate_setsize(inode, cifsi->server_eof);
	fscache_resize_cookie(cifs_inode_cookie(inode), cifsi->server_eof);
 out:
	free_xid(xid);
	return rc;
}