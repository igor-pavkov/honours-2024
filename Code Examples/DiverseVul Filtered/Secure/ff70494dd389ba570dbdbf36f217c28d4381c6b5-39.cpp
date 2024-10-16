static char *parse_timestamp(unsigned long long timestamp) {
	static char s[15];
	unsigned long long rem;
	unsigned int days, hours, mins, secs;

	days = timestamp / TSTP_DAY;
	rem = timestamp % TSTP_DAY;
	hours = rem / TSTP_HOUR;
	rem %= TSTP_HOUR;
	mins = rem / TSTP_MIN;
	rem %= TSTP_MIN;
	secs = rem / TSTP_SEC;

	snprintf(s, 14, "%3dd %02d:%02d:%02d", days, hours, mins, secs);

	return s;
}