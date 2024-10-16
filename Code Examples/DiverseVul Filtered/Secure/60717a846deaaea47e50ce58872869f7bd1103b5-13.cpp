static void __ntfs_attr_init(ntfs_attr *na, ntfs_inode *ni,
		const ATTR_TYPES type, ntfschar *name, const u32 name_len)
{
	na->rl = NULL;
	na->ni = ni;
	na->type = type;
	na->name = name;
	if (name)
		na->name_len = name_len;
	else
		na->name_len = 0;
}