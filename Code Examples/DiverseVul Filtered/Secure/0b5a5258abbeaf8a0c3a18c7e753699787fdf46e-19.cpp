  Event_creation_ctx(CHARSET_INFO *client_cs,
                     CHARSET_INFO *connection_cl,
                     CHARSET_INFO *db_cl)
    : Stored_program_creation_ctx(client_cs, connection_cl, db_cl)
  { }