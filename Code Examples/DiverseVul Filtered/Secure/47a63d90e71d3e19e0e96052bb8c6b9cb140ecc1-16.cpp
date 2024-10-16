int xattr_diff(struct file_struct *file, stat_x *sxp, int find_all)
{
	const rsync_xa_list *glst = rsync_xal_l.items;
	const item_list *lst;
	rsync_xa *snd_rxa, *rec_rxa;
	int snd_cnt, rec_cnt;
	int cmp, same, xattrs_equal = 1;

	if (sxp && XATTR_READY(*sxp)) {
		rec_rxa = sxp->xattr->items;
		rec_cnt = sxp->xattr->count;
	} else {
		rec_rxa = NULL;
		rec_cnt = 0;
	}

	if (F_XATTR(file) >= 0) {
		glst += F_XATTR(file);
		lst = &glst->xa_items;
	} else
		lst = &empty_xattr;

	snd_rxa = lst->items;
	snd_cnt = lst->count;

	/* If the count of the sender's xattrs is different from our
	 * (receiver's) xattrs, the lists are not the same. */
	if (snd_cnt != rec_cnt) {
		if (!find_all)
			return 1;
		xattrs_equal = 0;
	}

	while (snd_cnt) {
		cmp = rec_cnt ? strcmp(snd_rxa->name, rec_rxa->name) : -1;
		if (cmp > 0)
			same = 0;
		else if (snd_rxa->datum_len > MAX_FULL_DATUM) {
			same = cmp == 0 && snd_rxa->datum_len == rec_rxa->datum_len
			    && memcmp(snd_rxa->datum + 1, rec_rxa->datum + 1,
				      MAX_DIGEST_LEN) == 0;
			/* Flag unrequested items that we need. */
			if (!same && find_all && snd_rxa->datum[0] == XSTATE_ABBREV)
				snd_rxa->datum[0] = XSTATE_TODO;
		} else {
			same = cmp == 0 && snd_rxa->datum_len == rec_rxa->datum_len
			    && memcmp(snd_rxa->datum, rec_rxa->datum,
				      snd_rxa->datum_len) == 0;
		}
		if (!same) {
			if (!find_all)
				return 1;
			xattrs_equal = 0;
		}

		if (cmp <= 0) {
			snd_rxa++;
			snd_cnt--;
		}
		if (cmp >= 0) {
			rec_rxa++;
			rec_cnt--;
		}
	}

	if (rec_cnt)
		xattrs_equal = 0;

	return !xattrs_equal;
}