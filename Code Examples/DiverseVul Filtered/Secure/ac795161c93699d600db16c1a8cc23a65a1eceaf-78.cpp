static bool nfs_verify_change_attribute(struct inode *dir, unsigned long verf)
{
	return (verf & ~1UL) == nfs_save_change_attribute(dir);
}