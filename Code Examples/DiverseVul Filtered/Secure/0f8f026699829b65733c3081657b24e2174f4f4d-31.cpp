static time_t parse_datetime(char *datestr, char *timestr, char *gmt)
{
    int datelen = strlen(datestr), leapday;
    unsigned long d, t;
    char *p;
    struct tm tm;

    /* check format of strings */
    if ((datelen != 6 && datelen != 8) ||
	strlen(timestr) != 6 || (gmt && strcasecmp(gmt, "GMT")))
	return -1;

    /* convert datestr to ulong */
    d = strtoul(datestr, &p, 10);
    if (d == ULONG_MAX || *p) return -1;

    /* convert timestr to ulong */
    t = strtoul(timestr, &p, 10);
    if (t == ULONG_MAX || *p) return -1;

    /* populate the time struct */
    tm.tm_year = d / 10000;
    d %= 10000;
    tm.tm_mon = d / 100 - 1;
    tm.tm_mday = d % 100;

    tm.tm_hour = t / 10000;
    t %= 10000;
    tm.tm_min = t / 100;
    tm.tm_sec = t % 100;

    /* massage the year to years since 1900 */
    if (tm.tm_year > 99) tm.tm_year -= 1900;
    else {
	/*
	 * guess century
	 * if year > current year, use previous century
	 * otherwise, use current century
	 */
	time_t now = time(NULL);
	struct tm *current;
	int century;

        current = gmt ? gmtime(&now) : localtime(&now);
        century = current->tm_year / 100;
        if (tm.tm_year > current->tm_year % 100) century--;
        tm.tm_year += century * 100;
    }

    /* sanity check the date/time (including leap day and leap second) */
    leapday = tm.tm_mon == 1 && isleap(tm.tm_year + 1900);
    if (tm.tm_year < 70 || tm.tm_mon < 0 || tm.tm_mon > 11 ||
	tm.tm_mday < 1 || tm.tm_mday > (numdays[tm.tm_mon] + leapday) ||
	tm.tm_hour > 23 || tm.tm_min > 59 || tm.tm_sec > 60)
        return -1;

    return (gmt ? mkgmtime(&tm) : mktime(&tm));
}