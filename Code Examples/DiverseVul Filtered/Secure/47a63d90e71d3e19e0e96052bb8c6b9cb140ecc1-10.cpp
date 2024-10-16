static int rsync_xal_get(const char *fname, item_list *xalp)
{
	ssize_t list_len, name_len;
	size_t datum_len, name_offset;
	char *name, *ptr;
#ifdef HAVE_LINUX_XATTRS
	int user_only = am_sender ? 0 : !am_root;
#endif
	rsync_xa *rxa;
	int count;

	/* This puts the name list into the "namebuf" buffer. */
	if ((list_len = get_xattr_names(fname)) < 0)
		return -1;

	for (name = namebuf; list_len > 0; name += name_len) {
		name_len = strlen(name) + 1;
		list_len -= name_len;

		if (saw_xattr_filter) {
			if (name_is_excluded(name, NAME_IS_XATTR, ALL_FILTERS))
				continue;
		}
#ifdef HAVE_LINUX_XATTRS
		/* Choose between ignoring the system namespace or (non-root) ignoring any non-user namespace. */
		else if (user_only ? !HAS_PREFIX(name, USER_PREFIX) : HAS_PREFIX(name, SYSTEM_PREFIX))
			continue;
#endif

		/* No rsync.%FOO attributes are copied w/o 2 -X options. */
		if (name_len > RPRE_LEN && name[RPRE_LEN] == '%' && HAS_PREFIX(name, RSYNC_PREFIX)) {
			if ((am_sender && preserve_xattrs < 2)
			 || (am_root < 0
			  && (strcmp(name+RPRE_LEN+1, XSTAT_SUFFIX) == 0
			   || strcmp(name+RPRE_LEN+1, XACC_ACL_SUFFIX) == 0
			   || strcmp(name+RPRE_LEN+1, XDEF_ACL_SUFFIX) == 0)))
				continue;
		}

		datum_len = name_len; /* Pass extra size to get_xattr_data() */
		if (!(ptr = get_xattr_data(fname, name, &datum_len, 0)))
			return -1;

		if (datum_len > MAX_FULL_DATUM) {
			/* For large datums, we store a flag and a checksum. */
			name_offset = 1 + MAX_DIGEST_LEN;
			sum_init(-1, checksum_seed);
			sum_update(ptr, datum_len);
			free(ptr);

			if (!(ptr = new_array(char, name_offset + name_len)))
				out_of_memory("rsync_xal_get");
			*ptr = XSTATE_ABBREV;
			sum_end(ptr + 1);
		} else
			name_offset = datum_len;

		rxa = EXPAND_ITEM_LIST(xalp, rsync_xa, RSYNC_XAL_INITIAL);
		rxa->name = ptr + name_offset;
		memcpy(rxa->name, name, name_len);
		rxa->datum = ptr;
		rxa->name_len = name_len;
		rxa->datum_len = datum_len;
	}
	count = xalp->count;
	rxa = xalp->items;
	if (count > 1)
		qsort(rxa, count, sizeof (rsync_xa), rsync_xal_compare_names);
	for (rxa += count-1; count; count--, rxa--)
		rxa->num = count;
	return 0;
}