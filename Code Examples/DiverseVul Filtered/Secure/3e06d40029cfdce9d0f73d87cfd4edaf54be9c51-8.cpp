int get_tmpname(char *fnametmp, const char *fname, BOOL make_unique)
{
	int maxname, length = 0;
	const char *f;
	char *suf;

	if (tmpdir) {
		/* Note: this can't overflow, so the return value is safe */
		length = strlcpy(fnametmp, tmpdir, MAXPATHLEN - 2);
		fnametmp[length++] = '/';
	}

	if ((f = strrchr(fname, '/')) != NULL) {
		++f;
		if (!tmpdir) {
			length = f - fname;
			/* copy up to and including the slash */
			strlcpy(fnametmp, fname, length + 1);
		}
	} else
		f = fname;

	if (!tmpdir) { /* using a tmpdir avoids the leading dot on our temp names */
		if (*f == '.') /* avoid an extra leading dot for OS X's sake */
			f++;
		fnametmp[length++] = '.';
	}

	/* The maxname value is bufsize, and includes space for the '\0'.
	 * NAME_MAX needs an extra -1 for the name's leading dot. */
	maxname = MIN(MAXPATHLEN - length - TMPNAME_SUFFIX_LEN,
		      NAME_MAX - 1 - TMPNAME_SUFFIX_LEN);

	if (maxname < 0) {
		rprintf(FERROR_XFER, "temporary filename too long: %s\n", fname);
		fnametmp[0] = '\0';
		return 0;
	}

	if (maxname) {
		int added = strlcpy(fnametmp + length, f, maxname);
		if (added >= maxname)
			added = maxname - 1;
		suf = fnametmp + length + added;

		/* Trim any dangling high-bit chars if the first-trimmed char (if any) is
		 * also a high-bit char, just in case we cut into a multi-byte sequence.
		 * We are guaranteed to stop because of the leading '.' we added. */
		if ((int)f[added] & 0x80) {
			while ((int)suf[-1] & 0x80)
				suf--;
		}
		/* trim one trailing dot before our suffix's dot */
		if (suf[-1] == '.')
			suf--;
	} else
		suf = fnametmp + length - 1; /* overwrite the leading dot with suffix's dot */

	if (make_unique) {
		static unsigned counter_limit;
		unsigned counter;

		if (!counter_limit) {
			counter_limit = (unsigned)getpid() + MAX_UNIQUE_LOOP;
			if (counter_limit > MAX_UNIQUE_NUMBER || counter_limit < MAX_UNIQUE_LOOP)
				counter_limit = MAX_UNIQUE_LOOP;
		}
		counter = counter_limit - MAX_UNIQUE_LOOP;

		/* This doesn't have to be very good because we don't need
		 * to worry about someone trying to guess the values:  all
		 * a conflict will do is cause a device, special file, hard
		 * link, or symlink to fail to be created.  Also: avoid
		 * using mktemp() due to gcc's annoying warning. */
		while (1) {
			snprintf(suf, TMPNAME_SUFFIX_LEN+1, ".%d", counter);
			if (access(fnametmp, 0) < 0)
				break;
			if (++counter >= counter_limit)
				return 0;
		}
	} else
		memcpy(suf, TMPNAME_SUFFIX, TMPNAME_SUFFIX_LEN+1);

	return 1;
}