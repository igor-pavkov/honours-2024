static void test_set_option()
{
  MYSQL_STMT *stmt;
  MYSQL_RES  *result;
  int        rc;

  myheader("test_set_option");

  mysql_autocommit(mysql, TRUE);

  /* LIMIT the rows count to 2 */
  rc= mysql_query(mysql, "SET OPTION SQL_SELECT_LIMIT= 2");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_limit");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_limit(a tinyint)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_limit VALUES(10), (20), (30), (40)");
  myquery(rc);

  if (!opt_silent)
    fprintf(stdout, "\n with SQL_SELECT_LIMIT= 2 (direct)");
  rc= mysql_query(mysql, "SELECT * FROM test_limit");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 2);

  mysql_free_result(result);

  if (!opt_silent)
    fprintf(stdout, "\n with SQL_SELECT_LIMIT=2 (prepare)");
  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_limit");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 2);

  mysql_stmt_close(stmt);

  /* RESET the LIMIT the rows count to 0 */
  if (!opt_silent)
    fprintf(stdout, "\n with SQL_SELECT_LIMIT=DEFAULT (prepare)");
  rc= mysql_query(mysql, "SET OPTION SQL_SELECT_LIMIT=DEFAULT");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_limit");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 4);

  mysql_stmt_close(stmt);
}