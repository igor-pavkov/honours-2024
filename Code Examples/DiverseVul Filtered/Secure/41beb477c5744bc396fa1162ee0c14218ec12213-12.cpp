time_t dos_to_unix_time(dosdatetime)
    ulg dosdatetime;
{
    time_t m_time;

#ifdef HAVE_MKTIME

    ZCONST time_t now = time(NULL);
    struct tm *tm;
#   define YRBASE  1900

    tm = localtime(&now);
    tm->tm_isdst = -1;          /* let mktime determine if DST is in effect */

    /* dissect date */
    tm->tm_year = ((int)(dosdatetime >> 25) & 0x7f) + (1980 - YRBASE);
    tm->tm_mon  = ((int)(dosdatetime >> 21) & 0x0f) - 1;
    tm->tm_mday = ((int)(dosdatetime >> 16) & 0x1f);

    /* dissect time */
    tm->tm_hour = (int)((unsigned)dosdatetime >> 11) & 0x1f;
    tm->tm_min  = (int)((unsigned)dosdatetime >> 5) & 0x3f;
    tm->tm_sec  = (int)((unsigned)dosdatetime << 1) & 0x3e;

    m_time = mktime(tm);
    NATIVE_TO_TIMET(m_time)     /* NOP unless MSC 7.0 or Macintosh */
    TTrace((stderr, "  final m_time  =       %lu\n", (ulg)m_time));

#else /* !HAVE_MKTIME */

    int yr, mo, dy, hh, mm, ss;
#ifdef TOPS20
#   define YRBASE  1900
    struct tmx *tmx;
    char temp[20];
#else /* !TOPS20 */
#   define YRBASE  1970
    int leap;
    unsigned days;
    struct tm *tm;
#if (!defined(MACOS) && !defined(RISCOS) && !defined(QDOS) && !defined(TANDEM))
#ifdef WIN32
    TIME_ZONE_INFORMATION tzinfo;
    DWORD res;
#else /* ! WIN32 */
#ifndef BSD4_4   /* GRR:  change to !defined(MODERN) ? */
#if (defined(BSD) || defined(MTS) || defined(__GO32__))
    struct timeb tbp;
#else /* !(BSD || MTS || __GO32__) */
#ifdef DECLARE_TIMEZONE
    extern time_t timezone;
#endif
#endif /* ?(BSD || MTS || __GO32__) */
#endif /* !BSD4_4 */
#endif /* ?WIN32 */
#endif /* !MACOS && !RISCOS && !QDOS && !TANDEM */
#endif /* ?TOPS20 */


    /* dissect date */
    yr = ((int)(dosdatetime >> 25) & 0x7f) + (1980 - YRBASE);
    mo = ((int)(dosdatetime >> 21) & 0x0f) - 1;
    dy = ((int)(dosdatetime >> 16) & 0x1f) - 1;

    /* dissect time */
    hh = (int)((unsigned)dosdatetime >> 11) & 0x1f;
    mm = (int)((unsigned)dosdatetime >> 5) & 0x3f;
    ss = (int)((unsigned)dosdatetime & 0x1f) * 2;

#ifdef TOPS20
    tmx = (struct tmx *)malloc(sizeof(struct tmx));
    sprintf (temp, "%02d/%02d/%02d %02d:%02d:%02d", mo+1, dy+1, yr, hh, mm, ss);
    time_parse(temp, tmx, (char *)0);
    m_time = time_make(tmx);
    free(tmx);

#else /* !TOPS20 */

/*---------------------------------------------------------------------------
    Calculate the number of seconds since the epoch, usually 1 January 1970.
  ---------------------------------------------------------------------------*/

    /* leap = # of leap yrs from YRBASE up to but not including current year */
    leap = ((yr + YRBASE - 1) / 4);   /* leap year base factor */

    /* calculate days from BASE to this year and add expired days this year */
    days = (yr * 365) + (leap - 492) + ydays[mo];

    /* if year is a leap year and month is after February, add another day */
    if ((mo > 1) && ((yr+YRBASE)%4 == 0) && ((yr+YRBASE) != 2100))
        ++days;                 /* OK through 2199 */

    /* convert date & time to seconds relative to 00:00:00, 01/01/YRBASE */
    m_time = (time_t)((unsigned long)(days + dy) * 86400L +
                      (unsigned long)hh * 3600L +
                      (unsigned long)(mm * 60 + ss));
      /* - 1;   MS-DOS times always rounded up to nearest even second */
    TTrace((stderr, "dos_to_unix_time:\n"));
    TTrace((stderr, "  m_time before timezone = %lu\n", (ulg)m_time));

/*---------------------------------------------------------------------------
    Adjust for local standard timezone offset.
  ---------------------------------------------------------------------------*/

#if (!defined(MACOS) && !defined(RISCOS) && !defined(QDOS) && !defined(TANDEM))
#ifdef WIN32
    /* account for timezone differences */
    res = GetTimeZoneInformation(&tzinfo);
    if (res != TIME_ZONE_ID_INVALID)
    {
    m_time += 60*(tzinfo.Bias);
#else /* !WIN32 */
#if (defined(BSD) || defined(MTS) || defined(__GO32__))
#ifdef BSD4_4
    if ( (dosdatetime >= DOSTIME_2038_01_18) &&
         (m_time < (time_t)0x70000000L) )
        m_time = U_TIME_T_MAX;  /* saturate in case of (unsigned) overflow */
    if (m_time < (time_t)0L)    /* a converted DOS time cannot be negative */
        m_time = S_TIME_T_MAX;  /*  -> saturate at max signed time_t value */
    if ((tm = localtime(&m_time)) != (struct tm *)NULL)
        m_time -= tm->tm_gmtoff;                /* sec. EAST of GMT: subtr. */
#else /* !(BSD4_4 */
    ftime(&tbp);                                /* get `timezone' */
    m_time += tbp.timezone * 60L;               /* seconds WEST of GMT:  add */
#endif /* ?(BSD4_4 || __EMX__) */
#else /* !(BSD || MTS || __GO32__) */
    /* tzset was already called at start of process_zipfiles() */
    /* tzset(); */              /* set `timezone' variable */
#ifndef __BEOS__                /* BeOS DR8 has no timezones... */
    m_time += timezone;         /* seconds WEST of GMT:  add */
#endif
#endif /* ?(BSD || MTS || __GO32__) */
#endif /* ?WIN32 */
    TTrace((stderr, "  m_time after timezone =  %lu\n", (ulg)m_time));

/*---------------------------------------------------------------------------
    Adjust for local daylight savings (summer) time.
  ---------------------------------------------------------------------------*/

#ifndef BSD4_4  /* (DST already added to tm_gmtoff, so skip tm_isdst) */
    if ( (dosdatetime >= DOSTIME_2038_01_18) &&
         (m_time < (time_t)0x70000000L) )
        m_time = U_TIME_T_MAX;  /* saturate in case of (unsigned) overflow */
    if (m_time < (time_t)0L)    /* a converted DOS time cannot be negative */
        m_time = S_TIME_T_MAX;  /*  -> saturate at max signed time_t value */
    TIMET_TO_NATIVE(m_time)     /* NOP unless MSC 7.0 or Macintosh */
    if (((tm = localtime((time_t *)&m_time)) != NULL) && tm->tm_isdst)
#ifdef WIN32
        m_time += 60L * tzinfo.DaylightBias;    /* adjust with DST bias */
    else
        m_time += 60L * tzinfo.StandardBias;    /* add StdBias (normally 0) */
#else
        m_time -= 60L * 60L;    /* adjust for daylight savings time */
#endif
    NATIVE_TO_TIMET(m_time)     /* NOP unless MSC 7.0 or Macintosh */
    TTrace((stderr, "  m_time after DST =       %lu\n", (ulg)m_time));
#endif /* !BSD4_4 */
#ifdef WIN32
    }
#endif
#endif /* !MACOS && !RISCOS && !QDOS && !TANDEM */
#endif /* ?TOPS20 */

#endif /* ?HAVE_MKTIME */

    if ( (dosdatetime >= DOSTIME_2038_01_18) &&
         (m_time < (time_t)0x70000000L) )
        m_time = U_TIME_T_MAX;  /* saturate in case of (unsigned) overflow */
    if (m_time < (time_t)0L)    /* a converted DOS time cannot be negative */
        m_time = S_TIME_T_MAX;  /*  -> saturate at max signed time_t value */

    return m_time;

} /* end function dos_to_unix_time() */