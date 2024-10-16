void jfs_truncate(struct inode *ip)
{
	jfs_info("jfs_truncate: size = 0x%lx", (ulong) ip->i_size);

	nobh_truncate_page(ip->i_mapping, ip->i_size, jfs_get_block);

	IWRITE_LOCK(ip, RDWRLOCK_NORMAL);
	jfs_truncate_nolock(ip, ip->i_size);
	IWRITE_UNLOCK(ip);
}