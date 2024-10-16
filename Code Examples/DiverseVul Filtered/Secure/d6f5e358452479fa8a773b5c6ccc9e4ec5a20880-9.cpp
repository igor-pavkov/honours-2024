static long smb3_insert_range(struct file *file, struct cifs_tcon *tcon,
			      loff_t off, loff_t len)
{
	int rc;
	unsigned int xid;
	struct cifsFileInfo *cfile = file->private_data;
	__le64 eof;
	__u64  count;

	xid = get_xid();

	if (off >= i_size_read(file->f_inode)) {
		rc = -EINVAL;
		goto out;
	}

	count = i_size_read(file->f_inode) - off;
	eof = cpu_to_le64(i_size_read(file->f_inode) + len);

	rc = SMB2_set_eof(xid, tcon, cfile->fid.persistent_fid,
			  cfile->fid.volatile_fid, cfile->pid, &eof);
	if (rc < 0)
		goto out;

	rc = smb2_copychunk_range(xid, cfile, cfile, off, count, off + len);
	if (rc < 0)
		goto out;

	rc = smb3_zero_range(file, tcon, off, len, 1);
	if (rc < 0)
		goto out;

	rc = 0;
 out:
	free_xid(xid);
	return rc;
}