void free_xattr(stat_x *sxp)
{
	if (!sxp->xattr)
		return;
	rsync_xal_free(sxp->xattr);
	free(sxp->xattr);
	sxp->xattr = NULL;
}