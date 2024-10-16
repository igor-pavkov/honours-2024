int sa_get_record_timestamp_struct(uint64_t l_flags, struct record_header *record_hdr,
				   struct tm *rectime)
{
	struct tm *ltm;
	int rc = 0;

	/*
	 * Fill generic rectime structure in local time.
	 * Done so that we have some default values.
	 */
	ltm = localtime_r((const time_t *) &(record_hdr->ust_time), rectime);

	if (!PRINT_LOCAL_TIME(l_flags) && !PRINT_TRUE_TIME(l_flags)) {
		/*
		 * Get time in UTC
		 * (the user doesn't want local time nor time of file's creator).
		 */
		ltm = gmtime_r((const time_t *) &(record_hdr->ust_time), rectime);
	}

	if (!ltm) {
		rc = 1;
	}

	if (PRINT_TRUE_TIME(l_flags)) {
		/* Time of file's creator */
		rectime->tm_hour = record_hdr->hour;
		rectime->tm_min  = record_hdr->minute;
		rectime->tm_sec  = record_hdr->second;
	}

	return rc;
}