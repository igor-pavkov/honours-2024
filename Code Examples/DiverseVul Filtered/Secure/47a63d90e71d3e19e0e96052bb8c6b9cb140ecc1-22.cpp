int set_xattr(const char *fname, const struct file_struct *file,
	      const char *fnamecmp, stat_x *sxp)
{
	rsync_xa_list *glst = rsync_xal_l.items;
	item_list *lst;
	int ndx;

	if (dry_run)
		return 1; /* FIXME: --dry-run needs to compute this value */

	if (read_only || list_only) {
		errno = EROFS;
		return -1;
	}

#ifdef NO_SPECIAL_XATTRS
	if (IS_SPECIAL(sxp->st.st_mode)) {
		errno = ENOTSUP;
		return -1;
	}
#endif
#ifdef NO_DEVICE_XATTRS
	if (IS_DEVICE(sxp->st.st_mode)) {
		errno = ENOTSUP;
		return -1;
	}
#endif
#ifdef NO_SYMLINK_XATTRS
	if (S_ISLNK(sxp->st.st_mode)) {
		errno = ENOTSUP;
		return -1;
	}
#endif

	ndx = F_XATTR(file);
	glst += ndx;
	lst = &glst->xa_items;
	return rsync_xal_set(fname, lst, fnamecmp, sxp);
}