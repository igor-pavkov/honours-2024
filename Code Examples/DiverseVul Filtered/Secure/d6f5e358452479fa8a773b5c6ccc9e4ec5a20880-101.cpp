static int smb3_fiemap(struct cifs_tcon *tcon,
		       struct cifsFileInfo *cfile,
		       struct fiemap_extent_info *fei, u64 start, u64 len)
{
	unsigned int xid;
	struct file_allocated_range_buffer in_data, *out_data;
	u32 out_data_len;
	int i, num, rc, flags, last_blob;
	u64 next;

	rc = fiemap_prep(d_inode(cfile->dentry), fei, start, &len, 0);
	if (rc)
		return rc;

	xid = get_xid();
 again:
	in_data.file_offset = cpu_to_le64(start);
	in_data.length = cpu_to_le64(len);

	rc = SMB2_ioctl(xid, tcon, cfile->fid.persistent_fid,
			cfile->fid.volatile_fid,
			FSCTL_QUERY_ALLOCATED_RANGES, true,
			(char *)&in_data, sizeof(in_data),
			1024 * sizeof(struct file_allocated_range_buffer),
			(char **)&out_data, &out_data_len);
	if (rc == -E2BIG) {
		last_blob = 0;
		rc = 0;
	} else
		last_blob = 1;
	if (rc)
		goto out;

	if (out_data_len && out_data_len < sizeof(struct file_allocated_range_buffer)) {
		rc = -EINVAL;
		goto out;
	}
	if (out_data_len % sizeof(struct file_allocated_range_buffer)) {
		rc = -EINVAL;
		goto out;
	}

	num = out_data_len / sizeof(struct file_allocated_range_buffer);
	for (i = 0; i < num; i++) {
		flags = 0;
		if (i == num - 1 && last_blob)
			flags |= FIEMAP_EXTENT_LAST;

		rc = fiemap_fill_next_extent(fei,
				le64_to_cpu(out_data[i].file_offset),
				le64_to_cpu(out_data[i].file_offset),
				le64_to_cpu(out_data[i].length),
				flags);
		if (rc < 0)
			goto out;
		if (rc == 1) {
			rc = 0;
			goto out;
		}
	}

	if (!last_blob) {
		next = le64_to_cpu(out_data[num - 1].file_offset) +
		  le64_to_cpu(out_data[num - 1].length);
		len = len - (next - start);
		start = next;
		goto again;
	}

 out:
	free_xid(xid);
	kfree(out_data);
	return rc;
}