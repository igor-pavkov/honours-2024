Event_job_data::construct_drop_event_sql(THD *thd, String *sp_sql)
{
  LEX_STRING buffer;
  const uint STATIC_SQL_LENGTH= 14;

  DBUG_ENTER("Event_job_data::construct_drop_event_sql");

  buffer.length= STATIC_SQL_LENGTH + name.length*2 + dbname.length*2;
  if (! (buffer.str= (char*) thd->alloc(buffer.length)))
    DBUG_RETURN(TRUE);

  sp_sql->set(buffer.str, buffer.length, system_charset_info);
  sp_sql->length(0);

  sp_sql->append(C_STRING_WITH_LEN("DROP EVENT "));
  append_identifier(thd, sp_sql, dbname.str, dbname.length);
  sp_sql->append('.');
  append_identifier(thd, sp_sql, name.str, name.length);

  DBUG_RETURN(thd->is_fatal_error);
}