s64 ntfs_attr_pread(ntfs_attr *na, const s64 pos, s64 count, void *b)
{
	s64 ret;
	
	if (!na || !na->ni || !na->ni->vol || !b || pos < 0 || count < 0) {
		errno = EINVAL;
		ntfs_log_perror("%s: na=%p  b=%p  pos=%lld  count=%lld",
				__FUNCTION__, na, b, (long long)pos,
				(long long)count);
		return -1;
	}
	
	ntfs_log_enter("Entering for inode %lld attr 0x%x pos %lld count "
		       "%lld\n", (unsigned long long)na->ni->mft_no,
		       le32_to_cpu(na->type), (long long)pos, (long long)count);

	ret = ntfs_attr_pread_i(na, pos, count, b);
	
	ntfs_log_leave("\n");
	return ret;
}