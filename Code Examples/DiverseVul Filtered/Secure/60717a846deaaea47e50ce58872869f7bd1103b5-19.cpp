static int ntfs_attr_fill_zero(ntfs_attr *na, s64 pos, s64 count)
{
	char *buf;
	s64 written, size, end = pos + count;
	s64 ofsi;
	const runlist_element *rli;
	ntfs_volume *vol;
	int ret = -1;

	ntfs_log_trace("pos %lld, count %lld\n", (long long)pos, 
		       (long long)count);
	
	if (!na || pos < 0 || count < 0) {
		errno = EINVAL;
		goto err_out;
	}
	
	buf = ntfs_calloc(NTFS_BUF_SIZE);
	if (!buf)
		goto err_out;
	
	rli = na->rl;
	ofsi = 0;
	vol = na->ni->vol;
	while (pos < end) {
		while (rli->length && (ofsi + (rli->length <<
	                        vol->cluster_size_bits) <= pos)) {
	                ofsi += (rli->length << vol->cluster_size_bits);
			rli++;
		}
		size = min(end - pos, NTFS_BUF_SIZE);
			/*
			 * If the zeroed block is fully within a hole,
			 * we need not write anything, so advance as far
			 * as possible within the hole.
			 */
		if ((rli->lcn == (LCN)LCN_HOLE)
		    && (ofsi <= pos)
		    && (ofsi + (rli->length << vol->cluster_size_bits)
				>= (pos + size))) {
			size = min(end - pos, ofsi - pos
				+ (rli->length << vol->cluster_size_bits));
			pos += size;
		} else {
			written = ntfs_rl_pwrite(vol, rli, ofsi, pos,
							size, buf);
			if (written <= 0) {
				ntfs_log_perror("Failed to zero space");
				goto err_free;
			}
			pos += written;
		}
	}
	
	ret = 0;
err_free:	
	free(buf);
err_out:
	return ret;	
}