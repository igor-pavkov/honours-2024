Event_job_data::load_from_row(THD *thd, TABLE *table)
{
  char *ptr;
  size_t len;
  LEX_STRING tz_name;

  DBUG_ENTER("Event_job_data::load_from_row");

  if (!table)
    DBUG_RETURN(TRUE);

  if (table->s->fields < ET_FIELD_COUNT)
    DBUG_RETURN(TRUE);

  if (load_string_fields(table->field,
                         ET_FIELD_DB, &dbname,
                         ET_FIELD_NAME, &name,
                         ET_FIELD_BODY, &body,
                         ET_FIELD_DEFINER, &definer,
                         ET_FIELD_TIME_ZONE, &tz_name,
                         ET_FIELD_COUNT))
    DBUG_RETURN(TRUE);

  if (load_time_zone(thd, tz_name))
    DBUG_RETURN(TRUE);

  Event_creation_ctx::load_from_db(thd, &mem_root, dbname.str, name.str, table,
                                   &creation_ctx);

  ptr= strchr(definer.str, '@');

  if (! ptr)
    ptr= definer.str;

  len= ptr - definer.str;
  definer_user.str= strmake_root(&mem_root, definer.str, len);
  definer_user.length= len;
  len= definer.length - len - 1;
  /* 1:because of @ */
  definer_host.str= strmake_root(&mem_root, ptr + 1, len);
  definer_host.length= len;

  sql_mode= (ulong) table->field[ET_FIELD_SQL_MODE]->val_int();

  DBUG_RETURN(FALSE);
}