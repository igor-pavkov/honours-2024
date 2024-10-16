static void test_fetch_offset()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char       data[11];
  ulong      length;
  int        rc;
  my_bool    is_null;


  myheader("test_fetch_offset");

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1(a char(10))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1 values('abcdefghij'), (null)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select * from t1");
  check_stmt(stmt);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)data;
  my_bind[0].buffer_length= 11;
  my_bind[0].is_null= &is_null;
  my_bind[0].length= &length;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute_r(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  data[0]= '\0';
  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s (%ld)", data, length);
  DIE_UNLESS(strncmp(data, "abcd", 4) == 0 && length == 10);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 5);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s (%ld)", data, length);
  DIE_UNLESS(strncmp(data, "fg", 2) == 0 && length == 10);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 9);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 0: %s (%ld)", data, length);
  DIE_UNLESS(strncmp(data, "j", 1) == 0 && length == 10);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  is_null= 0;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);

  DIE_UNLESS(is_null == 1);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute_r(stmt, rc);

  mysql_stmt_close(stmt);

  myquery(mysql_query(mysql, "drop table t1"));
}