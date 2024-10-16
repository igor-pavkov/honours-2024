static void test_bug43560(void)
{
  MYSQL*       conn;
  uint         rc;
  MYSQL_STMT   *stmt= 0;
  MYSQL_BIND   bind;
  my_bool      is_null= 0;
  char         buffer[256];
  const uint   BUFSIZE= sizeof(buffer);
  const char*  values[] = {"eins", "zwei", "drei", "viele", NULL};
  const char   insert_str[] = "INSERT INTO t1 (c2) VALUES (?)";
  unsigned long length;
  const unsigned int drop_db= opt_drop_db;

  DBUG_ENTER("test_bug43560");
  myheader("test_bug43560");

  /* Make sure we only run against a debug server. */
  if (!strstr(mysql->server_version, "debug"))
  {
    fprintf(stdout, "Skipping test_bug43560: server not DEBUG version\n");
    DBUG_VOID_RETURN;
  }
  if (opt_unix_socket)
  {
    fprintf(stdout, "Skipping test_bug43560: connected via UNIX socket\n");
    DBUG_VOID_RETURN;
  }
  /*
    Set up a separate connection for this test to avoid messing up the
    general MYSQL object used in other subtests. Use TCP protocol to avoid
    problems with the buffer semantics of AF_UNIX, and turn off auto reconnect.
  */
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_query(conn, "DROP TABLE IF EXISTS t1");
  myquery(rc);
  rc= mysql_query(conn,
    "CREATE TABLE t1 (c1 INT PRIMARY KEY AUTO_INCREMENT, c2 CHAR(10))");
  myquery(rc);

  stmt= mysql_stmt_init(conn);
  check_stmt(stmt);
  rc= mysql_stmt_prepare(stmt, insert_str, strlen(insert_str));
  check_execute(stmt, rc);

  bind.buffer_type= MYSQL_TYPE_STRING;
  bind.buffer_length= BUFSIZE;
  bind.buffer= buffer;
  bind.is_null= &is_null;
  bind.length= &length;
  rc= mysql_stmt_bind_param(stmt, &bind);
  check_execute(stmt, rc);

  /* First execute; should succeed. */
  strncpy(buffer, values[0], BUFSIZE);
  length= strlen(buffer);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* 
    Set up the server to close this session's server-side socket after
    next execution of prep statement.
  */
  rc= mysql_query(conn,"SET SESSION debug='+d,close_conn_after_stmt_execute'");
  myquery(rc);

  /* Second execute; should fail due to socket closed during execution. */
  strncpy(buffer, values[1], BUFSIZE);
  length= strlen(buffer);
  rc= mysql_stmt_execute(stmt);
  DIE_UNLESS(rc && mysql_stmt_errno(stmt) == CR_SERVER_LOST);

  /* 
    Third execute; should fail (connection already closed), or SIGSEGV in
    case of a Bug#43560 type regression in which case the whole test fails.
  */
  strncpy(buffer, values[2], BUFSIZE);
  length= strlen(buffer);
  rc= mysql_stmt_execute(stmt);
  DIE_UNLESS(rc && mysql_stmt_errno(stmt) == CR_SERVER_LOST);

  opt_drop_db= 0;
  client_disconnect(conn);
  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
  opt_drop_db= drop_db;

  DBUG_VOID_RETURN;
}