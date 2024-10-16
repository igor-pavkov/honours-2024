static void test_bug4079()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  const char *stmt_text;
  uint32 res;
  int rc;

  myheader("test_bug4079");

  /* Create and fill table */
  mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  mysql_query(mysql, "CREATE TABLE t1 (a int)");
  mysql_query(mysql, "INSERT INTO t1 VALUES (1), (2)");

  /* Prepare erroneous statement */
  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT 1 < (SELECT a FROM t1)";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  /* Execute the select statement */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* Bind input buffers */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *) &res;

  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc != 0 && rc != MYSQL_NO_DATA);
  if (!opt_silent)
    printf("Got error from mysql_stmt_fetch (as expected):\n%s\n",
           mysql_stmt_error(stmt));
  /* buggy version of libmysql hanged up here */
  mysql_stmt_close(stmt);
}