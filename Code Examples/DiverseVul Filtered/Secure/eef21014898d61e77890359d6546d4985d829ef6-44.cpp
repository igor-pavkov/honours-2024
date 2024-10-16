static void test_bug5126()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  int32 c1, c2;
  const char *stmt_text;
  int rc;

  myheader("test_bug5126");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "CREATE TABLE t1 (a mediumint, b int)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "INSERT INTO t1 VALUES (8386608, 1)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT a, b FROM t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* Bind output buffers */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= &c1;
  my_bind[1].buffer_type= MYSQL_TYPE_LONG;
  my_bind[1].buffer= &c2;

  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);
  DIE_UNLESS(c1 == 8386608 && c2 == 1);
  if (!opt_silent)
    printf("%ld, %ld\n", (long) c1, (long) c2);
  mysql_stmt_close(stmt);
}