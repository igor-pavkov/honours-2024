static bool check_msdfs_link(struct files_struct *dirfsp,
			     struct smb_filename *atname,
			     struct smb_filename *smb_fname)
{
	int saved_errno = errno;
	if(lp_host_msdfs() &&
		lp_msdfs_root(SNUM(dirfsp->conn)) &&
		is_msdfs_link(dirfsp, atname)) {

		/*
		 * Copy the returned stat struct from the relative
		 * to the full pathname.
		 */
		smb_fname->st = atname->st;

		DEBUG(5,("check_msdfs_link: Masquerading msdfs link %s "
			"as a directory\n",
			smb_fname->base_name));
		smb_fname->st.st_ex_mode =
			(smb_fname->st.st_ex_mode & 0xFFF) | S_IFDIR;
		errno = saved_errno;
		return true;
	}
	errno = saved_errno;
	return false;
}