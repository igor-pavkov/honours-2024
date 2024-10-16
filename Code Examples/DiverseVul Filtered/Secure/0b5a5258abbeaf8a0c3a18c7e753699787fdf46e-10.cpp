Event_job_data::construct_sp_sql(THD *thd, String *sp_sql)
{
  LEX_STRING buffer;
  const uint STATIC_SQL_LENGTH= 44;

  DBUG_ENTER("Event_job_data::construct_sp_sql");

  /*
    Allocate a large enough buffer on the thread execution memory
    root to avoid multiple [re]allocations on system heap
  */
  buffer.length= STATIC_SQL_LENGTH + name.length + body.length;
  if (! (buffer.str= (char*) thd->alloc(buffer.length)))
    DBUG_RETURN(TRUE);

  sp_sql->set(buffer.str, buffer.length, system_charset_info);
  sp_sql->length(0);


  sp_sql->append(C_STRING_WITH_LEN("CREATE "));
  sp_sql->append(C_STRING_WITH_LEN("PROCEDURE "));
  /*
    Let's use the same name as the event name to perhaps produce a
    better error message in case it is a part of some parse error.
    We're using append_identifier here to successfully parse
    events with reserved names.
  */
  append_identifier(thd, sp_sql, name.str, name.length);

  /*
    The default SQL security of a stored procedure is DEFINER. We
    have already activated the security context of the event, so
    let's execute the procedure with the invoker rights to save on
    resets of security contexts.
  */
  sp_sql->append(C_STRING_WITH_LEN("() SQL SECURITY INVOKER "));

  sp_sql->append(body.str, body.length);

  DBUG_RETURN(thd->is_fatal_error);
}