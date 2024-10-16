void set_record_timestamp_string(uint64_t l_flags, struct record_header *record_hdr,
				 char *cur_date, char *cur_time, int len, struct tm *rectime)
{
	/* Set cur_time date value */
	if (PRINT_SEC_EPOCH(l_flags) && cur_date) {
		sprintf(cur_time, "%llu", record_hdr->ust_time);
		strcpy(cur_date, "");
	}
	else {
		/*
		 * If options -T or -t have been used then cur_time is
		 * expressed in local time. Else it is expressed in UTC.
		 */
		if (cur_date) {
			strftime(cur_date, len, "%Y-%m-%d", rectime);
		}
		if (USE_PREFD_TIME_OUTPUT(l_flags)) {
			strftime(cur_time, len, "%X", rectime);
		}
		else {
			strftime(cur_time, len, "%H:%M:%S", rectime);
		}
	}
}