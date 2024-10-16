int get_xattr(const char *fname, stat_x *sxp)
{
	sxp->xattr = new(item_list);
	*sxp->xattr = empty_xattr;

	if (S_ISREG(sxp->st.st_mode) || S_ISDIR(sxp->st.st_mode)) {
		/* Everyone supports this. */
	} else if (S_ISLNK(sxp->st.st_mode)) {
#ifndef NO_SYMLINK_XATTRS
		if (!preserve_links)
#endif
			return 0;
	} else if (IS_SPECIAL(sxp->st.st_mode)) {
#ifndef NO_SPECIAL_XATTRS
		if (!preserve_specials)
#endif
			return 0;
	} else if (IS_DEVICE(sxp->st.st_mode)) {
#ifndef NO_DEVICE_XATTRS
		if (!preserve_devices)
#endif
			return 0;
	} else if (IS_MISSING_FILE(sxp->st))
		return 0;

	if (rsync_xal_get(fname, sxp->xattr) < 0) {
		free_xattr(sxp);
		return -1;
	}
	return 0;
}