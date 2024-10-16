bool get_next_time(const Time_zone *time_zone, my_time_t *next,
                   my_time_t start, my_time_t time_now,
                   int i_value, interval_type i_type)
{
  DBUG_ENTER("get_next_time");
  DBUG_PRINT("enter", ("start: %lu  now: %lu", (long) start, (long) time_now));

  DBUG_ASSERT(start <= time_now);

  longlong months=0, seconds=0;

  switch (i_type) {
  case INTERVAL_YEAR:
    months= i_value*12;
    break;
  case INTERVAL_QUARTER:
    /* Has already been converted to months */
  case INTERVAL_YEAR_MONTH:
  case INTERVAL_MONTH:
    months= i_value;
    break;
  case INTERVAL_WEEK:
    /* WEEK has already been converted to days */
  case INTERVAL_DAY:
    seconds= i_value*24*3600;
    break;
  case INTERVAL_DAY_HOUR:
  case INTERVAL_HOUR:
    seconds= i_value*3600;
    break;
  case INTERVAL_DAY_MINUTE:
  case INTERVAL_HOUR_MINUTE:
  case INTERVAL_MINUTE:
    seconds= i_value*60;
    break;
  case INTERVAL_DAY_SECOND:
  case INTERVAL_HOUR_SECOND:
  case INTERVAL_MINUTE_SECOND:
  case INTERVAL_SECOND:
    seconds= i_value;
    break;
  case INTERVAL_DAY_MICROSECOND:
  case INTERVAL_HOUR_MICROSECOND:
  case INTERVAL_MINUTE_MICROSECOND:
  case INTERVAL_SECOND_MICROSECOND:
  case INTERVAL_MICROSECOND:
    /*
     We should return an error here so SHOW EVENTS/ SELECT FROM I_S.EVENTS
     would give an error then.
    */
    DBUG_RETURN(1);
  case INTERVAL_LAST:
    DBUG_ASSERT(0);
  }
  DBUG_PRINT("info", ("seconds: %ld  months: %ld", (long) seconds, (long) months));

  MYSQL_TIME local_start;
  MYSQL_TIME local_now;

  /* Convert times from UTC to local. */
  {
    time_zone->gmt_sec_to_TIME(&local_start, start);
    time_zone->gmt_sec_to_TIME(&local_now, time_now);
  }

  INTERVAL interval;
  bzero(&interval, sizeof(interval));
  my_time_t next_time= 0;

  if (seconds)
  {
    longlong seconds_diff;
    long microsec_diff;
    bool negative= calc_time_diff(&local_now, &local_start, 1,
                                  &seconds_diff, &microsec_diff);
    if (!negative)
    {
      /*
        The formula below returns the interval that, when added to
        local_start, will always give the time in the future.
      */
      interval.second= seconds_diff - seconds_diff % seconds + seconds;
      next_time= add_interval(&local_start, time_zone,
                              INTERVAL_SECOND, interval);
      if (next_time == 0)
        goto done;
    }

    if (next_time <= time_now)
    {
      /*
        If 'negative' is true above, then 'next_time == 0', and
        'next_time <= time_now' is also true.  If negative is false,
        then next_time was set, but perhaps to the value that is less
        then time_now.  See below for elaboration.
      */
      DBUG_ASSERT(negative || next_time > 0);

      /*
        If local_now < local_start, i.e. STARTS time is in the future
        according to the local time (it always in the past according
        to UTC---this is a prerequisite of this function), then
        STARTS is almost always in the past according to the local
        time too.  However, in the time zone that has backward
        Daylight Saving Time shift, the following may happen: suppose
        we have a backward DST shift at certain date after 2:59:59,
        i.e. local time goes 1:59:59, 2:00:00, ... , 2:59:59, (shift
        here) 2:00:00 (again), ... , 2:59:59 (again), 3:00:00, ... .
        Now suppose the time has passed the first 2:59:59, has been
        shifted backward, and now is (the second) 2:20:00.  The user
        does CREATE EVENT with STARTS 'current-date 2:40:00'.  Local
        time 2:40:00 from create statement is treated by time
        functions as the first such time, so according to UTC it comes
        before the second 2:20:00.  But according to local time it is
        obviously in the future, so we end up in this branch.

        Since we are in the second pass through 2:00:00--2:59:59, and
        any local time form this interval is treated by system
        functions as the time from the first pass, we have to find the
        time for the next execution that is past the DST-affected
        interval (past the second 2:59:59 for our example,
        i.e. starting from 3:00:00).  We do this in the loop until the
        local time is mapped onto future UTC time.  'start' time is in
        the past, so we may use 'do { } while' here, and add the first
        interval right away.

        Alternatively, it could be that local_now >= local_start.  Now
        for the example above imagine we do CREATE EVENT with STARTS
        'current-date 2:10:00'.  Local start 2:10 is in the past (now
        is local 2:20), so we add an interval, and get next execution
        time, say, 2:40.  It is in the future according to local time,
        but, again, since we are in the second pass through
        2:00:00--2:59:59, 2:40 will be converted into UTC time in the
        past.  So we will end up in this branch again, and may add
        intervals in a 'do { } while' loop.

        Note that for any given event we may end up here only if event
        next execution time will map to the time interval that is
        passed twice, and only if the server was started during the
        second pass, or the event is being created during the second
        pass.  After that, we never will get here (unless we again
        start the server during the second pass).  In other words,
        such a condition is extremely rare.
      */
      interval.second= seconds;
      do
      {
        next_time= add_interval(&local_start, time_zone,
                                INTERVAL_SECOND, interval);
        if (next_time == 0)
          goto done;
      }
      while (next_time <= time_now);
    }
  }
  else
  {
    long diff_months= ((long) local_now.year - (long) local_start.year)*12 +
                      ((long) local_now.month - (long) local_start.month);

    /*
      Unlike for seconds above, the formula below returns the interval
      that, when added to the local_start, will give the time in the
      past, or somewhere in the current month.  We are interested in
      the latter case, to see if this time has already passed, or is
      yet to come this month.

      Note that the time is guaranteed to be in the past unless
      (diff_months % months == 0), but no good optimization is
      possible here, because (diff_months % months == 0) is what will
      happen most of the time, as get_next_time() will be called right
      after the execution of the event.  We could pass last_executed
      time to this function, and see if the execution has already
      happened this month, but for that we will have to convert
      last_executed from seconds since epoch to local broken-down
      time, and this will greatly reduce the effect of the
      optimization.  So instead we keep the code simple and clean.
    */
    interval.month= (ulong) (diff_months - diff_months % months);
    next_time= add_interval(&local_start, time_zone,
                            INTERVAL_MONTH, interval);
    if (next_time == 0)
      goto done;

    if (next_time <= time_now)
    {
      interval.month= (ulong) months;
      next_time= add_interval(&local_start, time_zone,
                              INTERVAL_MONTH, interval);
      if (next_time == 0)
        goto done;
    }
  }

  DBUG_ASSERT(time_now < next_time);

  *next= next_time;

done:
  DBUG_PRINT("info", ("next_time: %ld", (long) next_time));
  DBUG_RETURN(next_time == 0);
}