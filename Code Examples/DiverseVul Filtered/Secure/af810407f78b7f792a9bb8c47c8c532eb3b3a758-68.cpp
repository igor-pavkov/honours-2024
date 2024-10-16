int handler::update_auto_increment()
{
  ulonglong nr, nb_reserved_values;
  bool append= FALSE;
  THD *thd= table->in_use;
  struct system_variables *variables= &thd->variables;
  int result=0, tmp;
  DBUG_ENTER("handler::update_auto_increment");

  /*
    next_insert_id is a "cursor" into the reserved interval, it may go greater
    than the interval, but not smaller.
  */
  DBUG_ASSERT(next_insert_id >= auto_inc_interval_for_cur_row.minimum());

  if ((nr= table->next_number_field->val_int()) != 0 ||
      (table->auto_increment_field_not_null &&
       thd->variables.sql_mode & MODE_NO_AUTO_VALUE_ON_ZERO))
  {

    /*
      There could be an error reported because value was truncated
      when strict mode is enabled.
    */
    if (thd->is_error())
      DBUG_RETURN(HA_ERR_AUTOINC_ERANGE);
    /*
      Update next_insert_id if we had already generated a value in this
      statement (case of INSERT VALUES(null),(3763),(null):
      the last NULL needs to insert 3764, not the value of the first NULL plus
      1).
      Ignore negative values.
    */
    if ((longlong) nr > 0 || (table->next_number_field->flags & UNSIGNED_FLAG))
      adjust_next_insert_id_after_explicit_value(nr);
    insert_id_for_cur_row= 0; // didn't generate anything
    DBUG_RETURN(0);
  }

  if (table->versioned())
  {
    Field *end= table->vers_end_field();
    DBUG_ASSERT(end);
    bitmap_set_bit(table->read_set, end->field_index);
    if (!end->is_max())
    {
      if (thd->lex->sql_command == SQLCOM_ALTER_TABLE)
      {
        if (!table->next_number_field->real_maybe_null())
          DBUG_RETURN(HA_ERR_UNSUPPORTED);
        table->next_number_field->set_null();
      }
      DBUG_RETURN(0);
    }
  }

  // ALTER TABLE ... ADD COLUMN ... AUTO_INCREMENT
  if (thd->lex->sql_command == SQLCOM_ALTER_TABLE)
    table->next_number_field->set_notnull();

  if ((nr= next_insert_id) >= auto_inc_interval_for_cur_row.maximum())
  {
    /* next_insert_id is beyond what is reserved, so we reserve more. */
    const Discrete_interval *forced=
      thd->auto_inc_intervals_forced.get_next();
    if (forced != NULL)
    {
      nr= forced->minimum();
      nb_reserved_values= forced->values();
    }
    else
    {
      /*
        handler::estimation_rows_to_insert was set by
        handler::ha_start_bulk_insert(); if 0 it means "unknown".
      */
      ulonglong nb_desired_values;
      /*
        If an estimation was given to the engine:
        - use it.
        - if we already reserved numbers, it means the estimation was
        not accurate, then we'll reserve 2*AUTO_INC_DEFAULT_NB_ROWS the 2nd
        time, twice that the 3rd time etc.
        If no estimation was given, use those increasing defaults from the
        start, starting from AUTO_INC_DEFAULT_NB_ROWS.
        Don't go beyond a max to not reserve "way too much" (because
        reservation means potentially losing unused values).
        Note that in prelocked mode no estimation is given.
      */

      if ((auto_inc_intervals_count == 0) && (estimation_rows_to_insert > 0))
        nb_desired_values= estimation_rows_to_insert;
      else if ((auto_inc_intervals_count == 0) &&
               (thd->lex->many_values.elements > 0))
      {
        /*
          For multi-row inserts, if the bulk inserts cannot be started, the
          handler::estimation_rows_to_insert will not be set. But we still
          want to reserve the autoinc values.
        */
        nb_desired_values= thd->lex->many_values.elements;
      }
      else /* go with the increasing defaults */
      {
        /* avoid overflow in formula, with this if() */
        if (auto_inc_intervals_count <= AUTO_INC_DEFAULT_NB_MAX_BITS)
        {
          nb_desired_values= AUTO_INC_DEFAULT_NB_ROWS *
            (1 << auto_inc_intervals_count);
          set_if_smaller(nb_desired_values, AUTO_INC_DEFAULT_NB_MAX);
        }
        else
          nb_desired_values= AUTO_INC_DEFAULT_NB_MAX;
      }
      get_auto_increment(variables->auto_increment_offset,
                         variables->auto_increment_increment,
                         nb_desired_values, &nr,
                         &nb_reserved_values);
      if (nr == ULONGLONG_MAX)
        DBUG_RETURN(HA_ERR_AUTOINC_READ_FAILED);  // Mark failure

      /*
        That rounding below should not be needed when all engines actually
        respect offset and increment in get_auto_increment(). But they don't
        so we still do it. Wonder if for the not-first-in-index we should do
        it. Hope that this rounding didn't push us out of the interval; even
        if it did we cannot do anything about it (calling the engine again
        will not help as we inserted no row).
      */
      nr= compute_next_insert_id(nr-1, variables);
    }

    if (table->s->next_number_keypart == 0)
    {
      /* We must defer the appending until "nr" has been possibly truncated */
      append= TRUE;
    }
    else
    {
      /*
        For such auto_increment there is no notion of interval, just a
        singleton. The interval is not even stored in
        thd->auto_inc_interval_for_cur_row, so we are sure to call the engine
        for next row.
      */
      DBUG_PRINT("info",("auto_increment: special not-first-in-index"));
    }
  }

  if (unlikely(nr == ULONGLONG_MAX))
      DBUG_RETURN(HA_ERR_AUTOINC_ERANGE);

  DBUG_ASSERT(nr != 0);
  DBUG_PRINT("info",("auto_increment: %llu  nb_reserved_values: %llu",
                     nr, append ? nb_reserved_values : 0));

  /* Store field without warning (Warning will be printed by insert) */
  {
    Check_level_instant_set check_level_save(thd, CHECK_FIELD_IGNORE);
    tmp= table->next_number_field->store((longlong)nr, TRUE);
  }

  if (unlikely(tmp))                            // Out of range value in store
  {
    /*
      First, test if the query was aborted due to strict mode constraints
      or new field value greater than maximum integer value:
    */
    if (thd->killed == KILL_BAD_DATA ||
        nr > table->next_number_field->get_max_int_value())
    {
      /*
        It's better to return an error here than getting a confusing
        'duplicate key error' later.
      */
      result= HA_ERR_AUTOINC_ERANGE;
    }
    else
    {
      /*
        Field refused this value (overflow) and truncated it, use the result
        of the truncation (which is going to be inserted); however we try to
        decrease it to honour auto_increment_* variables.
        That will shift the left bound of the reserved interval, we don't
        bother shifting the right bound (anyway any other value from this
        interval will cause a duplicate key).
      */
      nr= prev_insert_id(table->next_number_field->val_int(), variables);
      if (unlikely(table->next_number_field->store((longlong)nr, TRUE)))
        nr= table->next_number_field->val_int();
    }
  }
  if (append)
  {
    auto_inc_interval_for_cur_row.replace(nr, nb_reserved_values,
                                          variables->auto_increment_increment);
    auto_inc_intervals_count++;
    /* Row-based replication does not need to store intervals in binlog */
    if (((WSREP(thd) && wsrep_emulate_bin_log ) || mysql_bin_log.is_open())
        && !thd->is_current_stmt_binlog_format_row())
      thd->auto_inc_intervals_in_cur_stmt_for_binlog.
        append(auto_inc_interval_for_cur_row.minimum(),
               auto_inc_interval_for_cur_row.values(),
               variables->auto_increment_increment);
  }

  /*
    Record this autogenerated value. If the caller then
    succeeds to insert this value, it will call
    record_first_successful_insert_id_in_cur_stmt()
    which will set first_successful_insert_id_in_cur_stmt if it's not
    already set.
  */
  insert_id_for_cur_row= nr;

  if (result)                                   // overflow
    DBUG_RETURN(result);

  /*
    Set next insert id to point to next auto-increment value to be able to
    handle multi-row statements.
  */
  set_next_insert_id(compute_next_insert_id(nr, variables));

  DBUG_RETURN(0);
}