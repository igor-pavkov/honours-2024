static long smb3_simple_falloc(struct file *file, struct cifs_tcon *tcon,
			    loff_t off, loff_t len, bool keep_size)
{
	struct inode *inode;
	struct cifsInodeInfo *cifsi;
	struct cifsFileInfo *cfile = file->private_data;
	long rc = -EOPNOTSUPP;
	unsigned int xid;
	__le64 eof;

	xid = get_xid();

	inode = d_inode(cfile->dentry);
	cifsi = CIFS_I(inode);

	trace_smb3_falloc_enter(xid, cfile->fid.persistent_fid, tcon->tid,
				tcon->ses->Suid, off, len);
	/* if file not oplocked can't be sure whether asking to extend size */
	if (!CIFS_CACHE_READ(cifsi))
		if (keep_size == false) {
			trace_smb3_falloc_err(xid, cfile->fid.persistent_fid,
				tcon->tid, tcon->ses->Suid, off, len, rc);
			free_xid(xid);
			return rc;
		}

	/*
	 * Extending the file
	 */
	if ((keep_size == false) && i_size_read(inode) < off + len) {
		rc = inode_newsize_ok(inode, off + len);
		if (rc)
			goto out;

		if ((cifsi->cifsAttrs & FILE_ATTRIBUTE_SPARSE_FILE) == 0)
			smb2_set_sparse(xid, tcon, cfile, inode, false);

		eof = cpu_to_le64(off + len);
		rc = SMB2_set_eof(xid, tcon, cfile->fid.persistent_fid,
				  cfile->fid.volatile_fid, cfile->pid, &eof);
		if (rc == 0) {
			cifsi->server_eof = off + len;
			cifs_setsize(inode, off + len);
			cifs_truncate_page(inode->i_mapping, inode->i_size);
			truncate_setsize(inode, off + len);
		}
		goto out;
	}

	/*
	 * Files are non-sparse by default so falloc may be a no-op
	 * Must check if file sparse. If not sparse, and since we are not
	 * extending then no need to do anything since file already allocated
	 */
	if ((cifsi->cifsAttrs & FILE_ATTRIBUTE_SPARSE_FILE) == 0) {
		rc = 0;
		goto out;
	}

	if (keep_size == true) {
		/*
		 * We can not preallocate pages beyond the end of the file
		 * in SMB2
		 */
		if (off >= i_size_read(inode)) {
			rc = 0;
			goto out;
		}
		/*
		 * For fallocates that are partially beyond the end of file,
		 * clamp len so we only fallocate up to the end of file.
		 */
		if (off + len > i_size_read(inode)) {
			len = i_size_read(inode) - off;
		}
	}

	if ((keep_size == true) || (i_size_read(inode) >= off + len)) {
		/*
		 * At this point, we are trying to fallocate an internal
		 * regions of a sparse file. Since smb2 does not have a
		 * fallocate command we have two otions on how to emulate this.
		 * We can either turn the entire file to become non-sparse
		 * which we only do if the fallocate is for virtually
		 * the whole file,  or we can overwrite the region with zeroes
		 * using SMB2_write, which could be prohibitevly expensive
		 * if len is large.
		 */
		/*
		 * We are only trying to fallocate a small region so
		 * just write it with zero.
		 */
		if (len <= 1024 * 1024) {
			rc = smb3_simple_fallocate_range(xid, tcon, cfile,
							 off, len);
			goto out;
		}

		/*
		 * Check if falloc starts within first few pages of file
		 * and ends within a few pages of the end of file to
		 * ensure that most of file is being forced to be
		 * fallocated now. If so then setting whole file sparse
		 * ie potentially making a few extra pages at the beginning
		 * or end of the file non-sparse via set_sparse is harmless.
		 */
		if ((off > 8192) || (off + len + 8192 < i_size_read(inode))) {
			rc = -EOPNOTSUPP;
			goto out;
		}
	}

	smb2_set_sparse(xid, tcon, cfile, inode, false);
	rc = 0;

out:
	if (rc)
		trace_smb3_falloc_err(xid, cfile->fid.persistent_fid, tcon->tid,
				tcon->ses->Suid, off, len, rc);
	else
		trace_smb3_falloc_done(xid, cfile->fid.persistent_fid, tcon->tid,
				tcon->ses->Suid, off, len);

	free_xid(xid);
	return rc;
}