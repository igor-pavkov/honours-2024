static int smb3_simple_fallocate_write_range(unsigned int xid,
					     struct cifs_tcon *tcon,
					     struct cifsFileInfo *cfile,
					     loff_t off, loff_t len,
					     char *buf)
{
	struct cifs_io_parms io_parms = {0};
	int nbytes;
	int rc = 0;
	struct kvec iov[2];

	io_parms.netfid = cfile->fid.netfid;
	io_parms.pid = current->tgid;
	io_parms.tcon = tcon;
	io_parms.persistent_fid = cfile->fid.persistent_fid;
	io_parms.volatile_fid = cfile->fid.volatile_fid;

	while (len) {
		io_parms.offset = off;
		io_parms.length = len;
		if (io_parms.length > SMB2_MAX_BUFFER_SIZE)
			io_parms.length = SMB2_MAX_BUFFER_SIZE;
		/* iov[0] is reserved for smb header */
		iov[1].iov_base = buf;
		iov[1].iov_len = io_parms.length;
		rc = SMB2_write(xid, &io_parms, &nbytes, iov, 1);
		if (rc)
			break;
		if (nbytes > len)
			return -EINVAL;
		buf += nbytes;
		off += nbytes;
		len -= nbytes;
	}
	return rc;
}