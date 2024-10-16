ntfs_attr *ntfs_attr_open(ntfs_inode *ni, const ATTR_TYPES type,
		ntfschar *name, u32 name_len)
{
	ntfs_attr_search_ctx *ctx;
	ntfs_attr *na = NULL;
	ntfschar *newname = NULL;
	ATTR_RECORD *a;
	le16 cs;

	ntfs_log_enter("Entering for inode %lld, attr 0x%x.\n",
		       (unsigned long long)ni->mft_no, le32_to_cpu(type));
	
	if (!ni || !ni->vol || !ni->mrec) {
		errno = EINVAL;
		goto out;
	}
	na = ntfs_calloc(sizeof(ntfs_attr));
	if (!na)
		goto out;
	if (!name_len)
		name = (ntfschar*)NULL;
	if (name && name != AT_UNNAMED && name != NTFS_INDEX_I30) {
		/* A null char leads to a short name and unallocated bytes */
		if (ntfs_ucsnlen(name, name_len) != name_len) {
			ntfs_log_error("Null character in attribute name"
				" of inode %lld\n",(long long)ni->mft_no);
			goto err_out;
		}
		name = ntfs_ucsndup(name, name_len);
		if (!name)
			goto err_out;
		newname = name;
	}

	ctx = ntfs_attr_get_search_ctx(ni, NULL);
	if (!ctx)
		goto err_out;

	if (ntfs_attr_lookup(type, name, name_len, 0, 0, NULL, 0, ctx))
		goto put_err_out;

	a = ctx->attr;
	
	if (!name) {
		if (a->name_length) {
			name = ntfs_ucsndup((ntfschar*)((u8*)a + le16_to_cpu(
					a->name_offset)), a->name_length);
			if (!name)
				goto put_err_out;
			newname = name;
			name_len = a->name_length;
		} else {
			name = AT_UNNAMED;
			name_len = 0;
		}
	}
	
	__ntfs_attr_init(na, ni, type, name, name_len);
	
	/*
	 * Wipe the flags in case they are not zero for an attribute list
	 * attribute.  Windows does not complain about invalid flags and chkdsk
	 * does not detect or fix them so we need to cope with it, too.
	 */
	if (type == AT_ATTRIBUTE_LIST)
		a->flags = const_cpu_to_le16(0);

	if ((type == AT_DATA)
	   && (a->non_resident ? !a->initialized_size : !a->value_length)) {
		/*
		 * Define/redefine the compression state if stream is
		 * empty, based on the compression mark on parent
		 * directory (for unnamed data streams) or on current
		 * inode (for named data streams). The compression mark
		 * may change any time, the compression state can only
		 * change when stream is wiped out.
		 * 
		 * Also prevent compression on NTFS version < 3.0
		 * or cluster size > 4K or compression is disabled
		 */
		a->flags &= ~ATTR_COMPRESSION_MASK;
		if ((ni->flags & FILE_ATTR_COMPRESSED)
		    && (ni->vol->major_ver >= 3)
		    && NVolCompression(ni->vol)
		    && (ni->vol->cluster_size <= MAX_COMPRESSION_CLUSTER_SIZE))
			a->flags |= ATTR_IS_COMPRESSED;
	}
	
	cs = a->flags & (ATTR_IS_COMPRESSED | ATTR_IS_SPARSE);

	/* a file may be sparse though its unnamed data is not (cf $UsnJrnl) */
	if (na->type == AT_DATA && na->name == AT_UNNAMED &&
	    (((a->flags & ATTR_IS_SPARSE)     && !NAttrSparse(na)) ||
	     (!(a->flags & ATTR_IS_ENCRYPTED)  != !NAttrEncrypted(na)))) {
		errno = EIO;
		ntfs_log_perror("Inode %lld has corrupt attribute flags "
				"(0x%x <> 0x%x)",(unsigned long long)ni->mft_no,
				le16_to_cpu(a->flags), le32_to_cpu(na->ni->flags));
		goto put_err_out;
	}

	if (a->non_resident) {
		if (((a->flags & ATTR_COMPRESSION_MASK)
			|| a->compression_unit)
		    && (ni->vol->major_ver < 3)) {
			errno = EIO;
			ntfs_log_perror("Compressed inode %lld not allowed"
					" on NTFS %d.%d",
					(unsigned long long)ni->mft_no,
					ni->vol->major_ver,
					ni->vol->major_ver);
			goto put_err_out;
		}
		if ((a->flags & ATTR_COMPRESSION_MASK)
				 && !a->compression_unit) {
			errno = EIO;
			ntfs_log_perror("Compressed inode %lld attr 0x%x has "
					"no compression unit",
					(unsigned long long)ni->mft_no, le32_to_cpu(type));
			goto put_err_out;
		}
		if ((a->flags & ATTR_COMPRESSION_MASK)
				 && (a->compression_unit
					!= STANDARD_COMPRESSION_UNIT)) {
			errno = EIO;
			ntfs_log_perror("Compressed inode %lld attr 0x%lx has "
					"an unsupported compression unit %d",
					(unsigned long long)ni->mft_no,
					(long)le32_to_cpu(type),
					(int)a->compression_unit);
			goto put_err_out;
		}
		ntfs_attr_init(na, TRUE, a->flags,
				a->flags & ATTR_IS_ENCRYPTED,
				a->flags & ATTR_IS_SPARSE,
				sle64_to_cpu(a->allocated_size),
				sle64_to_cpu(a->data_size),
				sle64_to_cpu(a->initialized_size),
				cs ? sle64_to_cpu(a->compressed_size) : 0,
				cs ? a->compression_unit : 0);
	} else {
		s64 l = le32_to_cpu(a->value_length);
		ntfs_attr_init(na, FALSE, a->flags,
				a->flags & ATTR_IS_ENCRYPTED,
				a->flags & ATTR_IS_SPARSE, (l + 7) & ~7, l, l,
				cs ? (l + 7) & ~7 : 0, 0);
	}
	ntfs_attr_put_search_ctx(ctx);
out:
	ntfs_log_leave("\n");	
	return na;

put_err_out:
	ntfs_attr_put_search_ctx(ctx);
err_out:
	free(newname);
	free(na);
	na = NULL;
	goto out;
}