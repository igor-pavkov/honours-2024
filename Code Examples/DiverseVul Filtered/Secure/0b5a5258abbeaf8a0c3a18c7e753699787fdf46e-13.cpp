Event_creation_ctx::load_from_db(THD *thd,
                                 MEM_ROOT *event_mem_root,
                                 const char *db_name,
                                 const char *event_name,
                                 TABLE *event_tbl,
                                 Stored_program_creation_ctx **ctx)
{
  /* Load character set/collation attributes. */

  CHARSET_INFO *client_cs;
  CHARSET_INFO *connection_cl;
  CHARSET_INFO *db_cl;

  bool invalid_creation_ctx= FALSE;

  if (load_charset(event_mem_root,
                   event_tbl->field[ET_FIELD_CHARACTER_SET_CLIENT],
                   thd->variables.character_set_client,
                   &client_cs))
  {
    sql_print_warning("Event '%s'.'%s': invalid value "
                      "in column mysql.event.character_set_client.",
                      (const char *) db_name,
                      (const char *) event_name);

    invalid_creation_ctx= TRUE;
  }

  if (load_collation(event_mem_root,
                     event_tbl->field[ET_FIELD_COLLATION_CONNECTION],
                     thd->variables.collation_connection,
                     &connection_cl))
  {
    sql_print_warning("Event '%s'.'%s': invalid value "
                      "in column mysql.event.collation_connection.",
                      (const char *) db_name,
                      (const char *) event_name);

    invalid_creation_ctx= TRUE;
  }

  if (load_collation(event_mem_root,
                     event_tbl->field[ET_FIELD_DB_COLLATION],
                     NULL,
                     &db_cl))
  {
    sql_print_warning("Event '%s'.'%s': invalid value "
                      "in column mysql.event.db_collation.",
                      (const char *) db_name,
                      (const char *) event_name);

    invalid_creation_ctx= TRUE;
  }

  /*
    If we failed to resolve the database collation, load the default one
    from the disk.
  */

  if (!db_cl)
    db_cl= get_default_db_collation(thd, db_name);

  /* Create the context. */

  *ctx= new Event_creation_ctx(client_cs, connection_cl, db_cl);

  return invalid_creation_ctx;
}