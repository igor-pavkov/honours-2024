u64 gf_net_parse_date(const char *val)
{
	u64 current_time;
	char szDay[50], szMonth[50];
	u32 year, month, day, h, m, s, ms;
	s32 oh, om;
	Float secs;
	Bool neg_time_zone = GF_FALSE;

#ifdef _WIN32_WCE
	SYSTEMTIME syst;
	FILETIME filet;
#else
	struct tm t;
	memset(&t, 0, sizeof(struct tm));
#endif

	szDay[0] = szMonth[0] = 0;
	year = month = day = h = m = s = 0;
	oh = om = 0;
	secs = 0;

	if (sscanf(val, "%d-%d-%dT%d:%d:%gZ", &year, &month, &day, &h, &m, &secs) == 6) {
	}
	else if (sscanf(val, "%d-%d-%dT%d:%d:%g-%d:%d", &year, &month, &day, &h, &m, &secs, &oh, &om) == 8) {
		neg_time_zone = GF_TRUE;
	}
	else if (sscanf(val, "%d-%d-%dT%d:%d:%g+%d:%d", &year, &month, &day, &h, &m, &secs, &oh, &om) == 8) {
	}
	else if (sscanf(val, "%3s, %d %3s %d %d:%d:%d", szDay, &day, szMonth, &year, &h, &m, &s)==7) {
		secs  = (Float) s;
	}
	else if (sscanf(val, "%9s, %d-%3s-%d %02d:%02d:%02d GMT", szDay, &day, szMonth, &year, &h, &m, &s)==7) {
		secs  = (Float) s;
	}
	else if (sscanf(val, "%3s %3s %d %02d:%02d:%02d %d", szDay, szMonth, &day, &year, &h, &m, &s)==7) {
		secs  = (Float) s;
	}
	else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[Core] Cannot parse date string %s\n", val));
		return 0;
	}

	if (month) {
		month -= 1;
	} else {
		if (!strcmp(szMonth, "Jan")) month = 0;
		else if (!strcmp(szMonth, "Feb")) month = 1;
		else if (!strcmp(szMonth, "Mar")) month = 2;
		else if (!strcmp(szMonth, "Apr")) month = 3;
		else if (!strcmp(szMonth, "May")) month = 4;
		else if (!strcmp(szMonth, "Jun")) month = 5;
		else if (!strcmp(szMonth, "Jul")) month = 6;
		else if (!strcmp(szMonth, "Aug")) month = 7;
		else if (!strcmp(szMonth, "Sep")) month = 8;
		else if (!strcmp(szMonth, "Oct")) month = 9;
		else if (!strcmp(szMonth, "Nov")) month = 10;
		else if (!strcmp(szMonth, "Dec")) month = 11;
	}

#ifdef _WIN32_WCE
	memset(&syst, 0, sizeof(SYSTEMTIME));
	syst.wYear = year;
	syst.wMonth = month + 1;
	syst.wDay = day;
	syst.wHour = h;
	syst.wMinute = m;
	syst.wSecond = (u32) secs;
	SystemTimeToFileTime(&syst, &filet);
	current_time = (u64) ((*(LONGLONG *) &filet - TIMESPEC_TO_FILETIME_OFFSET) / 10000000);

#else

	t.tm_year = year>1000 ? year-1900 : year;
	t.tm_mday = day;
	t.tm_hour = h;
	t.tm_min = m;
	t.tm_sec = (u32) secs;
	t.tm_mon = month;

	if (strlen(szDay) ) {
		if (!strcmp(szDay, "Mon") || !strcmp(szDay, "Monday")) t.tm_wday = 0;
		else if (!strcmp(szDay, "Tue") || !strcmp(szDay, "Tuesday")) t.tm_wday = 1;
		else if (!strcmp(szDay, "Wed") || !strcmp(szDay, "Wednesday")) t.tm_wday = 2;
		else if (!strcmp(szDay, "Thu") || !strcmp(szDay, "Thursday")) t.tm_wday = 3;
		else if (!strcmp(szDay, "Fri") || !strcmp(szDay, "Friday")) t.tm_wday = 4;
		else if (!strcmp(szDay, "Sat") || !strcmp(szDay, "Saturday")) t.tm_wday = 5;
		else if (!strcmp(szDay, "Sun") || !strcmp(szDay, "Sunday")) t.tm_wday = 6;
	}

	current_time = gf_mktime_utc(&t);

	if ((s64) current_time == -1) {
		//use 1 ms
		return 1;
	}
	if (current_time == 0) {
		//use 1 ms
		return 1;
	}

#endif

	if (om || oh) {
		s32 diff = (60*oh + om)*60;
		if (neg_time_zone) diff = -diff;
		current_time = current_time + diff;
	}
	current_time *= 1000;
	ms = (u32) ( (secs - (u32) secs) * 1000);
	return current_time + ms;
}