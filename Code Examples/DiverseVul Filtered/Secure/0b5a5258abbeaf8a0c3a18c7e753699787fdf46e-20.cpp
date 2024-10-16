Event_timed::load_from_row(THD *thd, TABLE *table)
{
  char *ptr;
  size_t len;

  DBUG_ENTER("Event_timed::load_from_row");

  if (Event_queue_element::load_from_row(thd, table))
    DBUG_RETURN(TRUE);

  if (load_string_fields(table->field,
                         ET_FIELD_BODY, &body,
                         ET_FIELD_BODY_UTF8, &body_utf8,
                         ET_FIELD_COUNT))
    DBUG_RETURN(TRUE);

  if (Event_creation_ctx::load_from_db(thd, &mem_root, dbname.str, name.str,
                                       table, &creation_ctx))
  {
    push_warning_printf(thd,
                        Sql_condition::WARN_LEVEL_WARN,
                        ER_EVENT_INVALID_CREATION_CTX,
                        ER_THD(thd, ER_EVENT_INVALID_CREATION_CTX),
                        (const char *) dbname.str,
                        (const char *) name.str);
  }

  ptr= strchr(definer.str, '@');

  if (! ptr)
    ptr= definer.str;

  len= ptr - definer.str;
  definer_user.str= strmake_root(&mem_root, definer.str, len);
  definer_user.length= len;
  len= definer.length - len - 1;
  /* 1:because of @ */
  definer_host.str= strmake_root(&mem_root, ptr + 1,  len);
  definer_host.length= len;

  created= table->field[ET_FIELD_CREATED]->val_int();
  modified= table->field[ET_FIELD_MODIFIED]->val_int();

  comment.str= get_field(&mem_root, table->field[ET_FIELD_COMMENT]);
  if (comment.str != NullS)
    comment.length= strlen(comment.str);
  else
    comment.length= 0;

  sql_mode= (ulong) table->field[ET_FIELD_SQL_MODE]->val_int();

  DBUG_RETURN(FALSE);
}