static time_t php_openssl_asn1_time_to_time_t(ASN1_UTCTIME * timestr) /* {{{ */
{
/*
	This is how the time string is formatted:

   snprintf(p, sizeof(p), "%02d%02d%02d%02d%02d%02dZ",ts->tm_year%100,
	  ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);
*/

	time_t ret;
	struct tm thetime;
	char * strbuf;
	char * thestr;
	long gmadjust = 0;
	size_t timestr_len;

	if (ASN1_STRING_type(timestr) != V_ASN1_UTCTIME && ASN1_STRING_type(timestr) != V_ASN1_GENERALIZEDTIME) {
		php_error_docref(NULL, E_WARNING, "illegal ASN1 data type for timestamp");
		return (time_t)-1;
	}

	timestr_len = (size_t)ASN1_STRING_length(timestr);

	if (timestr_len != strlen((const char *)ASN1_STRING_get0_data(timestr))) {
		php_error_docref(NULL, E_WARNING, "illegal length in timestamp");
		return (time_t)-1;
	}

	if (timestr_len < 13 && timestr_len != 11) {
		php_error_docref(NULL, E_WARNING, "unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	if (ASN1_STRING_type(timestr) == V_ASN1_GENERALIZEDTIME && timestr_len < 15) {
		php_error_docref(NULL, E_WARNING, "unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	strbuf = estrdup((const char *)ASN1_STRING_get0_data(timestr));

	memset(&thetime, 0, sizeof(thetime));

	/* we work backwards so that we can use atoi more easily */

	thestr = strbuf + timestr_len - 3;

	if (timestr_len == 11) {
		thetime.tm_sec = 0;
	} else {
		thetime.tm_sec = atoi(thestr);
		*thestr = '\0';
		thestr -= 2;
	}
	thetime.tm_min = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_hour = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_mday = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_mon = atoi(thestr)-1;

	*thestr = '\0';
	if( ASN1_STRING_type(timestr) == V_ASN1_UTCTIME ) {
		thestr -= 2;
		thetime.tm_year = atoi(thestr);

		if (thetime.tm_year < 68) {
			thetime.tm_year += 100;
		}
	} else if( ASN1_STRING_type(timestr) == V_ASN1_GENERALIZEDTIME ) {
		thestr -= 4;
		thetime.tm_year = atoi(thestr) - 1900;
	}


	thetime.tm_isdst = -1;
	ret = mktime(&thetime);

#if HAVE_TM_GMTOFF
	gmadjust = thetime.tm_gmtoff;
#else
	/*
	** If correcting for daylight savings time, we set the adjustment to
	** the value of timezone - 3600 seconds. Otherwise, we need to overcorrect and
	** set the adjustment to the main timezone + 3600 seconds.
	*/
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}