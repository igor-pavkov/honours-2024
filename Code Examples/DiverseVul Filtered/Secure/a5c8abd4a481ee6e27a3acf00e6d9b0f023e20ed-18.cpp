int skip_extra_struct(int ifd, int endian_mismatch, int arch_64)
{
	int i;
	struct extra_desc xtra_d;

	do {
		/* Read extra structure description */
		sa_fread(ifd, &xtra_d, EXTRA_DESC_SIZE, HARD_SIZE, UEOF_STOP);

		/*
		 * We don't need to remap as the extra_desc structure won't change,
		 * but we may need to normalize endianness anyway.
		 */
		if (endian_mismatch) {
			swap_struct(extra_desc_types_nr, &xtra_d, arch_64);
		}

		/* Check values consistency */
		if (MAP_SIZE(xtra_d.extra_types_nr) > xtra_d.extra_size) {
#ifdef DEBUG
			fprintf(stderr, "%s: extra_size=%u types=%d,%d,%d\n",
				__FUNCTION__, xtra_d.extra_size,
				xtra_d.extra_types_nr[0], xtra_d.extra_types_nr[1], xtra_d.extra_types_nr[2]);
#endif
			return -1;
		}

		if ((xtra_d.extra_nr > MAX_EXTRA_NR) || (xtra_d.extra_size > MAX_EXTRA_SIZE)) {
#ifdef DEBUG
			fprintf(stderr, "%s: extra_size=%u extra_nr=%u\n",
				__FUNCTION__, xtra_d.extra_size, xtra_d.extra_size);
#endif
			return -1;
		}

		/* Ignore current unknown extra structures */
		for (i = 0; i < xtra_d.extra_nr; i++) {
			if (lseek(ifd, xtra_d.extra_size, SEEK_CUR) < xtra_d.extra_size)
				return -1;
		}
	}
	while (xtra_d.extra_next);

	return 0;
}