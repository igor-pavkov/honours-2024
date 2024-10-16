static s64 ntfs_attr_pread_i(ntfs_attr *na, const s64 pos, s64 count, void *b)
{
	s64 br, to_read, ofs, total, total2, max_read, max_init;
	ntfs_volume *vol;
	runlist_element *rl;
	u16 efs_padding_length;

	/* Sanity checking arguments is done in ntfs_attr_pread(). */
	
	if ((na->data_flags & ATTR_COMPRESSION_MASK) && NAttrNonResident(na)) {
		if ((na->data_flags & ATTR_COMPRESSION_MASK)
		    == ATTR_IS_COMPRESSED)
			return ntfs_compressed_attr_pread(na, pos, count, b);
		else {
				/* compression mode not supported */
			errno = EOPNOTSUPP;
			return -1;
		}
	}
	/*
	 * Encrypted non-resident attributes are not supported.  We return
	 * access denied, which is what Windows NT4 does, too.
	 * However, allow if mounted with efs_raw option
	 */
	vol = na->ni->vol;
	if (!vol->efs_raw && NAttrEncrypted(na) && NAttrNonResident(na)) {
		errno = EACCES;
		return -1;
	}
	
	if (!count)
		return 0;
		/*
		 * Truncate reads beyond end of attribute,
		 * but round to next 512 byte boundary for encrypted
		 * attributes with efs_raw mount option
		 */
	max_read = na->data_size;
	max_init = na->initialized_size;
	if (na->ni->vol->efs_raw
	    && (na->data_flags & ATTR_IS_ENCRYPTED)
	    && NAttrNonResident(na)) {
		if (na->data_size != na->initialized_size) {
			ntfs_log_error("uninitialized encrypted file not supported\n");
			errno = EINVAL;
			return -1;
		}	
		max_init = max_read = ((na->data_size + 511) & ~511) + 2;
	}
	if (pos + count > max_read) {
		if (pos >= max_read)
			return 0;
		count = max_read - pos;
	}
	/* If it is a resident attribute, get the value from the mft record. */
	if (!NAttrNonResident(na)) {
		ntfs_attr_search_ctx *ctx;
		char *val;

		ctx = ntfs_attr_get_search_ctx(na->ni, NULL);
		if (!ctx)
			return -1;
		if (ntfs_attr_lookup(na->type, na->name, na->name_len, 0,
				0, NULL, 0, ctx)) {
res_err_out:
			ntfs_attr_put_search_ctx(ctx);
			return -1;
		}
		val = (char*)ctx->attr + le16_to_cpu(ctx->attr->value_offset);
		if (val < (char*)ctx->attr || val +
				le32_to_cpu(ctx->attr->value_length) >
				(char*)ctx->mrec + vol->mft_record_size) {
			errno = EIO;
			ntfs_log_perror("%s: Sanity check failed", __FUNCTION__);
			goto res_err_out;
		}
		memcpy(b, val + pos, count);
		ntfs_attr_put_search_ctx(ctx);
		return count;
	}
	total = total2 = 0;
	/* Zero out reads beyond initialized size. */
	if (pos + count > max_init) {
		if (pos >= max_init) {
			memset(b, 0, count);
			return count;
		}
		total2 = pos + count - max_init;
		count -= total2;
		memset((u8*)b + count, 0, total2);
	}
		/*
		 * for encrypted non-resident attributes with efs_raw set 
		 * the last two bytes aren't read from disk but contain
		 * the number of padding bytes so original size can be 
		 * restored
		 */
	if (na->ni->vol->efs_raw && 
			(na->data_flags & ATTR_IS_ENCRYPTED) && 
			((pos + count) > max_init-2)) {
		efs_padding_length = 511 - ((na->data_size - 1) & 511);
		if (pos+count == max_init) {
			if (count == 1) {
				*((u8*)b+count-1) = (u8)(efs_padding_length >> 8);
				count--;
				total2++;
			} else {
				*(le16*)((u8*)b+count-2) = cpu_to_le16(efs_padding_length);
				count -= 2;
				total2 +=2;
			}
		} else {
			*((u8*)b+count-1) = (u8)(efs_padding_length & 0xff);
			count--;
			total2++;
		}
	}
	
	/* Find the runlist element containing the vcn. */
	rl = ntfs_attr_find_vcn(na, pos >> vol->cluster_size_bits);
	if (!rl) {
		/*
		 * If the vcn is not present it is an out of bounds read.
		 * However, we already truncated the read to the data_size,
		 * so getting this here is an error.
		 */
		if (errno == ENOENT) {
			errno = EIO;
			ntfs_log_perror("%s: Failed to find VCN #1", __FUNCTION__);
		}
		return -1;
	}
	/*
	 * Gather the requested data into the linear destination buffer. Note,
	 * a partial final vcn is taken care of by the @count capping of read
	 * length.
	 */
	ofs = pos - (rl->vcn << vol->cluster_size_bits);
	for (; count; rl++, ofs = 0) {
		if (rl->lcn == LCN_RL_NOT_MAPPED) {
			rl = ntfs_attr_find_vcn(na, rl->vcn);
			if (!rl) {
				if (errno == ENOENT) {
					errno = EIO;
					ntfs_log_perror("%s: Failed to find VCN #2",
							__FUNCTION__);
				}
				goto rl_err_out;
			}
			/* Needed for case when runs merged. */
			ofs = pos + total - (rl->vcn << vol->cluster_size_bits);
		}
		if (!rl->length) {
			errno = EIO;
			ntfs_log_perror("%s: Zero run length", __FUNCTION__);
			goto rl_err_out;
		}
		if (rl->lcn < (LCN)0) {
			if (rl->lcn != (LCN)LCN_HOLE) {
				ntfs_log_perror("%s: Bad run (%lld)", 
						__FUNCTION__,
						(long long)rl->lcn);
				goto rl_err_out;
			}
			/* It is a hole, just zero the matching @b range. */
			to_read = min(count, (rl->length <<
					vol->cluster_size_bits) - ofs);
			memset(b, 0, to_read);
			/* Update progress counters. */
			total += to_read;
			count -= to_read;
			b = (u8*)b + to_read;
			continue;
		}
		/* It is a real lcn, read it into @dst. */
		to_read = min(count, (rl->length << vol->cluster_size_bits) -
				ofs);
retry:
		ntfs_log_trace("Reading %lld bytes from vcn %lld, lcn %lld, ofs"
				" %lld.\n", (long long)to_read, (long long)rl->vcn,
			       (long long )rl->lcn, (long long)ofs);
		br = ntfs_pread(vol->dev, (rl->lcn << vol->cluster_size_bits) +
				ofs, to_read, b);
		/* If everything ok, update progress counters and continue. */
		if (br > 0) {
			total += br;
			count -= br;
			b = (u8*)b + br;
		}
		if (br == to_read)
			continue;
		/* If the syscall was interrupted, try again. */
		if (br == (s64)-1 && errno == EINTR)
			goto retry;
		if (total)
			return total;
		if (!br)
			errno = EIO;
		ntfs_log_perror("%s: ntfs_pread failed", __FUNCTION__);
		return -1;
	}
	/* Finally, return the number of bytes read. */
	return total + total2;
rl_err_out:
	if (total)
		return total;
	errno = EIO;
	return -1;
}