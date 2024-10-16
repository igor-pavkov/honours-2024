static void test_bug9643()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  int32 a;
  int rc;
  const char *stmt_text;
  int num_rows= 0;
  ulong type;
  ulong prefetch_rows= 5;

  myheader("test_bug9643");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (id integer not null primary key)");
  rc= mysql_query(mysql, "insert into t1 (id) values "
                         " (1), (2), (3), (4), (5), (6), (7), (8), (9)");
  myquery(rc);

  stmt= mysql_stmt_init(mysql);
  /* Not implemented in 5.0 */
  type= (ulong) CURSOR_TYPE_SCROLLABLE;
  rc= mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (void*) &type);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_stmt_error(stmt));

  type= (ulong) CURSOR_TYPE_READ_ONLY;
  rc= mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (void*) &type);
  check_execute(stmt, rc);
  rc= mysql_stmt_attr_set(stmt, STMT_ATTR_PREFETCH_ROWS,
                          (void*) &prefetch_rows);
  check_execute(stmt, rc);
  stmt_text= "select * from t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void*) &a;
  my_bind[0].buffer_length= sizeof(a);
  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  while ((rc= mysql_stmt_fetch(stmt)) == 0)
    ++num_rows;
  DIE_UNLESS(num_rows == 9);

  rc= mysql_stmt_close(stmt);
  DIE_UNLESS(rc == 0);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}