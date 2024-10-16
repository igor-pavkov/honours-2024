static void test_open_cursor_prepared_statement_query_cache()
{
  MYSQL_STMT *stmt;
  int rc;
  MYSQL_RES *result;

  myheader("test_open_cursor_prepared_statement_query_cache");
  if (! is_query_cache_available())
  {
    fprintf(stdout, "Skipping test_open_cursor_prepared_statement_query_cache: Query cache not available.\n");
    return;
  }

  rc= mysql_query(mysql, "set global query_cache_size=1000000");
  myquery(rc);

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (a int not null primary key, b int)");
  rc= mysql_query(mysql, "insert into t1 values (1,1)");
  myquery(rc);                                  /* one check is enough */

  /* Store query in query cache */
  rc= mysql_query(mysql, "SELECT * FROM t1");
  myquery(rc);
  result= mysql_store_result(mysql);
  mytest(result);
  (void) my_process_result_set(result);
  mysql_free_result(result);

  /* Test using a cursor */
  stmt= open_cursor("select a from t1");
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "set global query_cache_size=1000000");
  myquery(rc);
}