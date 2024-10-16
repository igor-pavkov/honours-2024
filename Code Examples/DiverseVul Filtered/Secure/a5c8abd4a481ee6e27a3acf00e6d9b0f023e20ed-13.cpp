void get_file_timestamp_struct(uint64_t flags, struct tm *rectime,
			       struct file_header *file_hdr)
{
	if (PRINT_TRUE_TIME(flags)) {
		/* Get local time. This is just to fill fields with a default value. */
		get_time(rectime, 0);

		rectime->tm_mday = file_hdr->sa_day;
		rectime->tm_mon  = file_hdr->sa_month;
		rectime->tm_year = file_hdr->sa_year;
		/*
		 * Call mktime() to set DST (Daylight Saving Time) flag.
		 * Has anyone a better way to do it?
		 */
		rectime->tm_hour = rectime->tm_min = rectime->tm_sec = 0;
		mktime(rectime);
	}
	else {
		localtime_r((const time_t *) &file_hdr->sa_ust_time, rectime);
	}
}