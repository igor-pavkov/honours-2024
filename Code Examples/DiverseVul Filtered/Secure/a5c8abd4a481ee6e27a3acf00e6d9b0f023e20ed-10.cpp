int read_record_hdr(int ifd, void *buffer, struct record_header *record_hdr,
		    struct file_header *file_hdr, int arch_64, int endian_mismatch,
		    int oneof, size_t b_size, uint64_t flags)
{
	int rc;

	do {
		if ((rc = sa_fread(ifd, buffer, (size_t) file_hdr->rec_size, SOFT_SIZE, oneof)) != 0)
			/* End of sa data file */
			return rc;

		/* Remap record header structure to that expected by current version */
		if (remap_struct(rec_types_nr, file_hdr->rec_types_nr, buffer,
				 file_hdr->rec_size, RECORD_HEADER_SIZE, b_size) < 0)
			return 2;
		memcpy(record_hdr, buffer, RECORD_HEADER_SIZE);

		/* Normalize endianness */
		if (endian_mismatch) {
			swap_struct(rec_types_nr, record_hdr, arch_64);
		}

		/* Raw output in debug mode */
		if (DISPLAY_DEBUG_MODE(flags)) {
			printf("# uptime_cs; %llu; ust_time; %llu; extra_next; %u; record_type; %d; HH:MM:SS; %02d:%02d:%02d\n",
			       record_hdr->uptime_cs, record_hdr->ust_time,
			       record_hdr->extra_next, record_hdr->record_type,
			       record_hdr->hour, record_hdr->minute, record_hdr->second);
		}

		/*
		 * Skip unknown extra structures if present.
		 * This will be done later for R_COMMENT and R_RESTART records, as extra structures
		 * are saved after the comment or the number of CPU.
		 */
		if ((record_hdr->record_type != R_COMMENT) && (record_hdr->record_type != R_RESTART) &&
		    record_hdr->extra_next && (skip_extra_struct(ifd, endian_mismatch, arch_64) < 0))
			return 2;
	}
	while ((record_hdr->record_type >= R_EXTRA_MIN) && (record_hdr->record_type <= R_EXTRA_MAX)) ;

	return 0;
}