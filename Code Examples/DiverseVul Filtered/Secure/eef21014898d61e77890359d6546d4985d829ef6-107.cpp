static void test_bug3117()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND buffer;
  longlong lii;
  ulong length;
  my_bool is_null;
  int rc;

  myheader("test_bug3117");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE t1 (id int auto_increment primary key)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "SELECT LAST_INSERT_ID()");
  check_stmt(stmt);

  rc= mysql_query(mysql, "INSERT INTO t1 VALUES (NULL)");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) &buffer, sizeof(buffer));
  buffer.buffer_type= MYSQL_TYPE_LONGLONG;
  buffer.buffer_length= sizeof(lii);
  buffer.buffer= (void *)&lii;
  buffer.length= &length;
  buffer.is_null= &is_null;

  rc= mysql_stmt_bind_result(stmt, &buffer);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(is_null == 0 && lii == 1);
  if (!opt_silent)
    fprintf(stdout, "\n\tLAST_INSERT_ID()= 1 ok\n");

  rc= mysql_query(mysql, "INSERT INTO t1 VALUES (NULL)");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(is_null == 0 && lii == 2);
  if (!opt_silent)
    fprintf(stdout, "\tLAST_INSERT_ID()= 2 ok\n");

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
}