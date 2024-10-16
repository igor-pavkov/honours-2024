Event_timed::get_create_event(THD *thd, String *buf)
{
  char tmp_buf[2 * STRING_BUFFER_USUAL_SIZE];
  String expr_buf(tmp_buf, sizeof(tmp_buf), system_charset_info);
  expr_buf.length(0);

  DBUG_ENTER("get_create_event");
  DBUG_PRINT("ret_info",("body_len=[%d]body=[%s]",
                         (int) body.length, body.str));

  if (expression && Events::reconstruct_interval_expression(&expr_buf, interval,
                                                            expression))
    DBUG_RETURN(EVEX_MICROSECOND_UNSUP);

  buf->append(STRING_WITH_LEN("CREATE "));
  append_definer(thd, buf, &definer_user, &definer_host);
  buf->append(STRING_WITH_LEN("EVENT "));
  append_identifier(thd, buf, name.str, name.length);

  if (expression)
  {
    buf->append(STRING_WITH_LEN(" ON SCHEDULE EVERY "));
    buf->append(expr_buf);
    buf->append(' ');
    LEX_STRING *ival= &interval_type_to_name[interval];
    buf->append(ival->str, ival->length);

    if (!starts_null)
      append_datetime(buf, time_zone, starts, STRING_WITH_LEN("STARTS"));

    if (!ends_null)
      append_datetime(buf, time_zone, ends, STRING_WITH_LEN("ENDS"));
  }
  else
  {
    append_datetime(buf, time_zone, execute_at,
                    STRING_WITH_LEN("ON SCHEDULE AT"));
  }

  if (on_completion == Event_parse_data::ON_COMPLETION_DROP)
    buf->append(STRING_WITH_LEN(" ON COMPLETION NOT PRESERVE "));
  else
    buf->append(STRING_WITH_LEN(" ON COMPLETION PRESERVE "));

  if (status == Event_parse_data::ENABLED)
    buf->append(STRING_WITH_LEN("ENABLE"));
  else if (status == Event_parse_data::SLAVESIDE_DISABLED)
    buf->append(STRING_WITH_LEN("DISABLE ON SLAVE"));
  else
    buf->append(STRING_WITH_LEN("DISABLE"));

  if (comment.length)
  {
    buf->append(STRING_WITH_LEN(" COMMENT "));
    append_unescaped(buf, comment.str, comment.length);
  }
  buf->append(STRING_WITH_LEN(" DO "));
  buf->append(body.str, body.length);

  DBUG_RETURN(0);
}