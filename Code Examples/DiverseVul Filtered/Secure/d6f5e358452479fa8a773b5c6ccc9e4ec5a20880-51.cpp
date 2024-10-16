static int smb3_simple_fallocate_range(unsigned int xid,
				       struct cifs_tcon *tcon,
				       struct cifsFileInfo *cfile,
				       loff_t off, loff_t len)
{
	struct file_allocated_range_buffer in_data, *out_data = NULL, *tmp_data;
	u32 out_data_len;
	char *buf = NULL;
	loff_t l;
	int rc;

	in_data.file_offset = cpu_to_le64(off);
	in_data.length = cpu_to_le64(len);
	rc = SMB2_ioctl(xid, tcon, cfile->fid.persistent_fid,
			cfile->fid.volatile_fid,
			FSCTL_QUERY_ALLOCATED_RANGES, true,
			(char *)&in_data, sizeof(in_data),
			1024 * sizeof(struct file_allocated_range_buffer),
			(char **)&out_data, &out_data_len);
	if (rc)
		goto out;

	buf = kzalloc(1024 * 1024, GFP_KERNEL);
	if (buf == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	tmp_data = out_data;
	while (len) {
		/*
		 * The rest of the region is unmapped so write it all.
		 */
		if (out_data_len == 0) {
			rc = smb3_simple_fallocate_write_range(xid, tcon,
					       cfile, off, len, buf);
			goto out;
		}

		if (out_data_len < sizeof(struct file_allocated_range_buffer)) {
			rc = -EINVAL;
			goto out;
		}

		if (off < le64_to_cpu(tmp_data->file_offset)) {
			/*
			 * We are at a hole. Write until the end of the region
			 * or until the next allocated data,
			 * whichever comes next.
			 */
			l = le64_to_cpu(tmp_data->file_offset) - off;
			if (len < l)
				l = len;
			rc = smb3_simple_fallocate_write_range(xid, tcon,
					       cfile, off, l, buf);
			if (rc)
				goto out;
			off = off + l;
			len = len - l;
			if (len == 0)
				goto out;
		}
		/*
		 * We are at a section of allocated data, just skip forward
		 * until the end of the data or the end of the region
		 * we are supposed to fallocate, whichever comes first.
		 */
		l = le64_to_cpu(tmp_data->length);
		if (len < l)
			l = len;
		off += l;
		len -= l;

		tmp_data = &tmp_data[1];
		out_data_len -= sizeof(struct file_allocated_range_buffer);
	}

 out:
	kfree(out_data);
	kfree(buf);
	return rc;
}