Event_queue_element::compute_next_execution_time()
{
  my_time_t time_now;
  DBUG_ENTER("Event_queue_element::compute_next_execution_time");
  DBUG_PRINT("enter", ("starts: %lu  ends: %lu  last_executed: %lu  this: %p",
                       (long) starts, (long) ends, (long) last_executed,
                       this));

  if (status != Event_parse_data::ENABLED)
  {
    DBUG_PRINT("compute_next_execution_time",
               ("Event %s is DISABLED", name.str));
    goto ret;
  }
  /* If one-time, no need to do computation */
  if (!expression)
  {
    /* Let's check whether it was executed */
    if (last_executed)
    {
      DBUG_PRINT("info",("One-time event %s.%s of was already executed",
                         dbname.str, name.str));
      dropped= (on_completion == Event_parse_data::ON_COMPLETION_DROP);
      DBUG_PRINT("info",("One-time event will be dropped: %d.", dropped));

      status= Event_parse_data::DISABLED;
    }
    goto ret;
  }

  time_now= current_thd->query_start();

  DBUG_PRINT("info",("NOW: [%lu]", (ulong) time_now));

  /* if time_now is after ends don't execute anymore */
  if (!ends_null && ends < time_now)
  {
    DBUG_PRINT("info", ("NOW after ENDS, don't execute anymore"));
    /* time_now is after ends. don't execute anymore */
    execute_at= 0;
    execute_at_null= TRUE;
    if (on_completion == Event_parse_data::ON_COMPLETION_DROP)
      dropped= TRUE;
    DBUG_PRINT("info", ("Dropped: %d", dropped));
    status= Event_parse_data::DISABLED;

    goto ret;
  }

  /*
    Here time_now is before or equals ends if the latter is set.
    Let's check whether time_now is before starts.
    If so schedule for starts.
  */
  if (!starts_null && time_now <= starts)
  {
    if (time_now == starts && starts == last_executed)
    {
      /*
        do nothing or we will schedule for second time execution at starts.
      */
    }
    else
    {
      DBUG_PRINT("info", ("STARTS is future, NOW <= STARTS,sched for STARTS"));
      /*
        starts is in the future
        time_now before starts. Scheduling for starts
      */
      execute_at= starts;
      execute_at_null= FALSE;
      goto ret;
    }
  }

  if (!starts_null && !ends_null)
  {
    /*
      Both starts and m_ends are set and time_now is between them (incl.)
      If last_executed is set then increase with m_expression. The new MYSQL_TIME is
      after m_ends set execute_at to 0. And check for on_completion
      If not set then schedule for now.
    */
    DBUG_PRINT("info", ("Both STARTS & ENDS are set"));
    if (!last_executed)
    {
      DBUG_PRINT("info", ("Not executed so far."));
    }

    {
      my_time_t next_exec;

      if (get_next_time(time_zone, &next_exec, starts, time_now,
                        (int) expression, interval))
        goto err;

      /* There was previous execution */
      if (ends < next_exec)
      {
        DBUG_PRINT("info", ("Next execution of %s after ENDS. Stop executing.",
                   name.str));
        /* Next execution after ends. No more executions */
        execute_at= 0;
        execute_at_null= TRUE;
        if (on_completion == Event_parse_data::ON_COMPLETION_DROP)
          dropped= TRUE;
        status= Event_parse_data::DISABLED;
      }
      else
      {
        DBUG_PRINT("info",("Next[%lu]", (ulong) next_exec));
        execute_at= next_exec;
        execute_at_null= FALSE;
      }
    }
    goto ret;
  }
  else if (starts_null && ends_null)
  {
    /* starts is always set, so this is a dead branch !! */
    DBUG_PRINT("info", ("Neither STARTS nor ENDS are set"));
    /*
      Both starts and m_ends are not set, so we schedule for the next
      based on last_executed.
    */
    if (last_executed)
    {
      my_time_t next_exec;
      if (get_next_time(time_zone, &next_exec, starts, time_now,
                        (int) expression, interval))
        goto err;
      execute_at= next_exec;
      DBUG_PRINT("info",("Next[%lu]", (ulong) next_exec));
    }
    else
    {
      /* last_executed not set. Schedule the event for now */
      DBUG_PRINT("info", ("Execute NOW"));
      execute_at= time_now;
    }
    execute_at_null= FALSE;
  }
  else
  {
    /* either starts or m_ends is set */
    if (!starts_null)
    {
      DBUG_PRINT("info", ("STARTS is set"));
      /*
        - starts is set.
        - starts is not in the future according to check made before
        Hence schedule for starts + m_expression in case last_executed
        is not set, otherwise to last_executed + m_expression
      */
      if (!last_executed)
      {
        DBUG_PRINT("info", ("Not executed so far."));
      }

      {
        my_time_t next_exec;
        if (get_next_time(time_zone, &next_exec, starts, time_now,
                          (int) expression, interval))
          goto err;
        execute_at= next_exec;
        DBUG_PRINT("info",("Next[%lu]", (ulong) next_exec));
      }
      execute_at_null= FALSE;
    }
    else
    {
      /* this is a dead branch, because starts is always set !!! */
      DBUG_PRINT("info", ("STARTS is not set. ENDS is set"));
      /*
        - m_ends is set
        - m_ends is after time_now or is equal
        Hence check for m_last_execute and increment with m_expression.
        If last_executed is not set then schedule for now
      */

      if (!last_executed)
        execute_at= time_now;
      else
      {
        my_time_t next_exec;

        if (get_next_time(time_zone, &next_exec, starts, time_now,
                          (int) expression, interval))
          goto err;

        if (ends < next_exec)
        {
          DBUG_PRINT("info", ("Next execution after ENDS. Stop executing."));
          execute_at= 0;
          execute_at_null= TRUE;
          status= Event_parse_data::DISABLED;
          if (on_completion == Event_parse_data::ON_COMPLETION_DROP)
            dropped= TRUE;
        }
        else
        {
          DBUG_PRINT("info", ("Next[%lu]", (ulong) next_exec));
          execute_at= next_exec;
          execute_at_null= FALSE;
        }
      }
    }
    goto ret;
  }
ret:
  DBUG_PRINT("info", ("ret: 0 execute_at: %lu", (long) execute_at));
  DBUG_RETURN(FALSE);
err:
  DBUG_PRINT("info", ("ret=1"));
  DBUG_RETURN(TRUE);
}