PJ_DEF(pj_status_t) pjmedia_rtcp_get_ntp_time(const pjmedia_rtcp_session *sess,
					      pjmedia_rtcp_ntp_rec *ntp)
{
/* Seconds between 1900-01-01 to 1970-01-01 */
#define JAN_1970  (2208988800UL)
    pj_timestamp ts;
    pj_status_t status;

    status = pj_get_timestamp(&ts);

    /* Fill up the high 32bit part */
    ntp->hi = (pj_uint32_t)((ts.u64 - sess->ts_base.u64) / sess->ts_freq.u64)
	      + sess->tv_base.sec + JAN_1970;

    /* Calculate seconds fractions */
    ts.u64 = (ts.u64 - sess->ts_base.u64) % sess->ts_freq.u64;
    pj_assert(ts.u64 < sess->ts_freq.u64);
    ts.u64 = (ts.u64 << 32) / sess->ts_freq.u64;

    /* Fill up the low 32bit part */
    ntp->lo = ts.u32.lo;


#if (defined(PJ_WIN32) && PJ_WIN32!=0) || \
    (defined(PJ_WIN64) && PJ_WIN64!=0) || \
    (defined(PJ_WIN32_WINCE) && PJ_WIN32_WINCE!=0)

    /* On Win32, since we use QueryPerformanceCounter() as the backend
     * timestamp API, we need to protect against this bug:
     *   Performance counter value may unexpectedly leap forward
     *   http://support.microsoft.com/default.aspx?scid=KB;EN-US;Q274323
     */
    {
	/*
	 * Compare elapsed time reported by timestamp with actual elapsed 
	 * time. If the difference is too excessive, then we use system
	 * time instead.
	 */

	/* MIN_DIFF needs to be large enough so that "normal" diff caused
	 * by system activity or context switch doesn't trigger the time
	 * correction.
	 */
	enum { MIN_DIFF = 400 };

	pj_time_val ts_time, elapsed, diff;

	pj_gettimeofday(&elapsed);

	ts_time.sec = ntp->hi - sess->tv_base.sec - JAN_1970;
	ts_time.msec = (long)(ntp->lo * 1000.0 / 0xFFFFFFFF);

	PJ_TIME_VAL_SUB(elapsed, sess->tv_base);

	if (PJ_TIME_VAL_LT(ts_time, elapsed)) {
	    diff = elapsed;
	    PJ_TIME_VAL_SUB(diff, ts_time);
	} else {
	    diff = ts_time;
	    PJ_TIME_VAL_SUB(diff, elapsed);
	}

	if (PJ_TIME_VAL_MSEC(diff) >= MIN_DIFF) {

	    TRACE_((sess->name, "RTCP NTP timestamp corrected by %d ms",
		    PJ_TIME_VAL_MSEC(diff)));


	    ntp->hi = elapsed.sec + sess->tv_base.sec + JAN_1970;
	    ntp->lo = (elapsed.msec * 65536 / 1000) << 16;
	}

    }
#endif

    return status;
}