static loff_t smb3_llseek(struct file *file, struct cifs_tcon *tcon, loff_t offset, int whence)
{
	struct cifsFileInfo *wrcfile, *cfile = file->private_data;
	struct cifsInodeInfo *cifsi;
	struct inode *inode;
	int rc = 0;
	struct file_allocated_range_buffer in_data, *out_data = NULL;
	u32 out_data_len;
	unsigned int xid;

	if (whence != SEEK_HOLE && whence != SEEK_DATA)
		return generic_file_llseek(file, offset, whence);

	inode = d_inode(cfile->dentry);
	cifsi = CIFS_I(inode);

	if (offset < 0 || offset >= i_size_read(inode))
		return -ENXIO;

	xid = get_xid();
	/*
	 * We need to be sure that all dirty pages are written as they
	 * might fill holes on the server.
	 * Note that we also MUST flush any written pages since at least
	 * some servers (Windows2016) will not reflect recent writes in
	 * QUERY_ALLOCATED_RANGES until SMB2_flush is called.
	 */
	wrcfile = find_writable_file(cifsi, FIND_WR_ANY);
	if (wrcfile) {
		filemap_write_and_wait(inode->i_mapping);
		smb2_flush_file(xid, tcon, &wrcfile->fid);
		cifsFileInfo_put(wrcfile);
	}

	if (!(cifsi->cifsAttrs & FILE_ATTRIBUTE_SPARSE_FILE)) {
		if (whence == SEEK_HOLE)
			offset = i_size_read(inode);
		goto lseek_exit;
	}

	in_data.file_offset = cpu_to_le64(offset);
	in_data.length = cpu_to_le64(i_size_read(inode));

	rc = SMB2_ioctl(xid, tcon, cfile->fid.persistent_fid,
			cfile->fid.volatile_fid,
			FSCTL_QUERY_ALLOCATED_RANGES, true,
			(char *)&in_data, sizeof(in_data),
			sizeof(struct file_allocated_range_buffer),
			(char **)&out_data, &out_data_len);
	if (rc == -E2BIG)
		rc = 0;
	if (rc)
		goto lseek_exit;

	if (whence == SEEK_HOLE && out_data_len == 0)
		goto lseek_exit;

	if (whence == SEEK_DATA && out_data_len == 0) {
		rc = -ENXIO;
		goto lseek_exit;
	}

	if (out_data_len < sizeof(struct file_allocated_range_buffer)) {
		rc = -EINVAL;
		goto lseek_exit;
	}
	if (whence == SEEK_DATA) {
		offset = le64_to_cpu(out_data->file_offset);
		goto lseek_exit;
	}
	if (offset < le64_to_cpu(out_data->file_offset))
		goto lseek_exit;

	offset = le64_to_cpu(out_data->file_offset) + le64_to_cpu(out_data->length);

 lseek_exit:
	free_xid(xid);
	kfree(out_data);
	if (!rc)
		return vfs_setpos(file, offset, inode->i_sb->s_maxbytes);
	else
		return rc;
}