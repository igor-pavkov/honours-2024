static void test_bug17512527()
{
  MYSQL *conn;
  MYSQL_STMT *stmt1, *stmt2;
  unsigned long thread_id;
  char query[MAX_TEST_QUERY_LENGTH];
  int rc;

  conn= client_connect(0, MYSQL_PROTOCOL_SOCKET, 1);

  stmt1 = mysql_stmt_init(conn);
  check_stmt(stmt1);
  rc= mysql_stmt_prepare(stmt1, STRING_WITH_LEN("SELECT 1"));
  check_execute(stmt1, rc);

  stmt2 = mysql_stmt_init(conn);
  check_stmt(stmt2);

  thread_id= mysql_thread_id(conn);
  sprintf(query, "KILL %lu", thread_id);
  if (thread_query(query))
    exit(1);

  rc= mysql_stmt_prepare(stmt2, STRING_WITH_LEN("SELECT 2"));
  check_execute(stmt2, rc);

  rc= mysql_stmt_execute(stmt1);
  check_execute_r(stmt1, rc);

  rc= mysql_stmt_execute(stmt2);
  check_execute(stmt2, rc);

  mysql_close(conn);

  mysql_stmt_close(stmt2);
  mysql_stmt_close(stmt1);
}