void udf_update_extra_perms(struct inode *inode, umode_t mode)
{
	struct udf_inode_info *iinfo = UDF_I(inode);

	/*
	 * UDF 2.01 sec. 3.3.3.3 Note 2:
	 * In Unix, delete permission tracks write
	 */
	iinfo->i_extraPerms &= ~FE_DELETE_PERMS;
	if (mode & 0200)
		iinfo->i_extraPerms |= FE_PERM_U_DELETE;
	if (mode & 0020)
		iinfo->i_extraPerms |= FE_PERM_G_DELETE;
	if (mode & 0002)
		iinfo->i_extraPerms |= FE_PERM_O_DELETE;
}