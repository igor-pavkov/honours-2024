static int rsync_xal_set(const char *fname, item_list *xalp,
			 const char *fnamecmp, stat_x *sxp)
{
	rsync_xa *rxas = xalp->items;
	ssize_t list_len;
	size_t i, len;
	char *name, *ptr, sum[MAX_DIGEST_LEN];
#ifdef HAVE_LINUX_XATTRS
	int user_only = am_root <= 0;
#endif
	size_t name_len;
	int ret = 0;

	/* This puts the current name list into the "namebuf" buffer. */
	if ((list_len = get_xattr_names(fname)) < 0)
		return -1;

	for (i = 0; i < xalp->count; i++) {
		name = rxas[i].name;

		if (XATTR_ABBREV(rxas[i])) {
			/* See if the fnamecmp version is identical. */
			len = name_len = rxas[i].name_len;
			if ((ptr = get_xattr_data(fnamecmp, name, &len, 1)) == NULL) {
			  still_abbrev:
				if (am_generator)
					continue;
				rprintf(FERROR, "Missing abbreviated xattr value, %s, for %s\n",
					rxas[i].name, full_fname(fname));
				ret = -1;
				continue;
			}
			if (len != rxas[i].datum_len) {
				free(ptr);
				goto still_abbrev;
			}

			sum_init(-1, checksum_seed);
			sum_update(ptr, len);
			sum_end(sum);
			if (memcmp(sum, rxas[i].datum + 1, MAX_DIGEST_LEN) != 0) {
				free(ptr);
				goto still_abbrev;
			}

			if (fname == fnamecmp)
				; /* Value is already set when identical */
			else if (sys_lsetxattr(fname, name, ptr, len) < 0) {
				rsyserr(FERROR_XFER, errno,
					"rsync_xal_set: lsetxattr(%s,\"%s\") failed",
					full_fname(fname), name);
				ret = -1;
			} else /* make sure caller sets mtime */
				sxp->st.st_mtime = (time_t)-1;

			if (am_generator) { /* generator items stay abbreviated */
				free(ptr);
				continue;
			}

			memcpy(ptr + len, name, name_len);
			free(rxas[i].datum);

			rxas[i].name = name = ptr + len;
			rxas[i].datum = ptr;
			continue;
		}

		if (sys_lsetxattr(fname, name, rxas[i].datum, rxas[i].datum_len) < 0) {
			rsyserr(FERROR_XFER, errno,
				"rsync_xal_set: lsetxattr(%s,\"%s\") failed",
				full_fname(fname), name);
			ret = -1;
		} else /* make sure caller sets mtime */
			sxp->st.st_mtime = (time_t)-1;
	}

	/* Remove any extraneous names. */
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
		if (am_root < 0 && name_len > RPRE_LEN && name[RPRE_LEN] == '%' && strcmp(name, XSTAT_ATTR) == 0)
			continue;

		for (i = 0; i < xalp->count; i++) {
			if (strcmp(name, rxas[i].name) == 0)
				break;
		}
		if (i == xalp->count) {
			if (sys_lremovexattr(fname, name) < 0) {
				rsyserr(FERROR_XFER, errno,
					"rsync_xal_set: lremovexattr(%s,\"%s\") failed",
					full_fname(fname), name);
				ret = -1;
			} else /* make sure caller sets mtime */
				sxp->st.st_mtime = (time_t)-1;
		}
	}

	return ret;
}