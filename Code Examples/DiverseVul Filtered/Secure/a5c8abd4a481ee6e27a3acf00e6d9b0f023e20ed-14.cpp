int sa_open_read_magic(int *fd, char *dfile, struct file_magic *file_magic,
		       int ignore, int *endian_mismatch, int do_swap)
{
	int n;
	unsigned int fm_types_nr[] = {FILE_MAGIC_ULL_NR, FILE_MAGIC_UL_NR, FILE_MAGIC_U_NR};

	/* Open sa data file */
	if ((*fd = open(dfile, O_RDONLY)) < 0) {
		int saved_errno = errno;

		fprintf(stderr, _("Cannot open %s: %s\n"), dfile, strerror(errno));

		if ((saved_errno == ENOENT) && default_file_used) {
			fprintf(stderr, _("Please check if data collecting is enabled\n"));
		}
		exit(2);
	}

	/* Read file magic data */
	n = read(*fd, file_magic, FILE_MAGIC_SIZE);

	if ((n != FILE_MAGIC_SIZE) ||
	    ((file_magic->sysstat_magic != SYSSTAT_MAGIC) && (file_magic->sysstat_magic != SYSSTAT_MAGIC_SWAPPED)) ||
	    ((file_magic->format_magic != FORMAT_MAGIC) && (file_magic->format_magic != FORMAT_MAGIC_SWAPPED) && !ignore)) {
#ifdef DEBUG
		fprintf(stderr, "%s: Bytes read=%d sysstat_magic=%x format_magic=%x\n",
			__FUNCTION__, n, file_magic->sysstat_magic, file_magic->format_magic);
#endif
		/* Display error message and exit */
		handle_invalid_sa_file(*fd, file_magic, dfile, n);
	}

	*endian_mismatch = (file_magic->sysstat_magic != SYSSTAT_MAGIC);
	if (*endian_mismatch) {
		if (do_swap) {
			/* Swap bytes for file_magic fields */
			file_magic->sysstat_magic = SYSSTAT_MAGIC;
			file_magic->format_magic  = __builtin_bswap16(file_magic->format_magic);
		}
		/*
		 * Start swapping at field "header_size" position.
		 * May not exist for older versions but in this case, it won't be used.
		 */
		swap_struct(fm_types_nr, &file_magic->header_size, 0);
	}

	if ((file_magic->sysstat_version > 10) ||
	    ((file_magic->sysstat_version == 10) && (file_magic->sysstat_patchlevel >= 3))) {
		/* header_size field exists only for sysstat versions 10.3.1 and later */
		if ((file_magic->header_size <= MIN_FILE_HEADER_SIZE) ||
		    (file_magic->header_size > MAX_FILE_HEADER_SIZE)) {
#ifdef DEBUG
			fprintf(stderr, "%s: header_size=%u\n",
				__FUNCTION__, file_magic->header_size);
#endif
			/* Display error message and exit */
			handle_invalid_sa_file(*fd, file_magic, dfile, n);
		}
	}
	if ((file_magic->sysstat_version > 11) ||
	    ((file_magic->sysstat_version == 11) && (file_magic->sysstat_patchlevel >= 7))) {
		/* hdr_types_nr field exists only for sysstat versions 11.7.1 and later */
		if (MAP_SIZE(file_magic->hdr_types_nr) > file_magic->header_size) {
#ifdef DEBUG
			fprintf(stderr, "%s: map_size=%u header_size=%u\n",
				__FUNCTION__, MAP_SIZE(file_magic->hdr_types_nr), file_magic->header_size);
#endif
			handle_invalid_sa_file(*fd, file_magic, dfile, n);
		}
	}

	if ((file_magic->format_magic != FORMAT_MAGIC) &&
	    (file_magic->format_magic != FORMAT_MAGIC_SWAPPED))
		/*
		 * This is an old (or new) sa datafile format to
		 * be read by sadf (since @ignore was set to TRUE).
		 */
		return -1;

	return 0;
}