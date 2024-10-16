static void test_free_store_result()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char       c2[5];
  ulong      bl1, l2;
  int        rc, c1, bc1;

  myheader("test_free_store_result");

  rc= mysql_query(mysql, "drop table if exists test_free_result");
  myquery(rc);

  rc= mysql_query(mysql, "create table test_free_result(c1 int primary key auto_increment)");
  myquery(rc);

  rc= mysql_query(mysql, "insert into test_free_result values(), (), ()");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select * from test_free_result");
  check_stmt(stmt);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&bc1;
  my_bind[0].buffer_length= 0;
  my_bind[0].is_null= 0;
  my_bind[0].length= &bl1;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  c2[0]= '\0'; l2= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)c2;
  my_bind[0].buffer_length= 7;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l2;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s(%ld)", c2, l2);
  DIE_UNLESS(strncmp(c2, "1", 1) == 0 && l2 == 1);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  c1= 0, l2= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&c1;
  my_bind[0].buffer_length= 0;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l2;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 0: %d(%ld)", c1, l2);
  DIE_UNLESS(c1 == 2 && l2 == 4);

  rc= mysql_stmt_free_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_query(mysql, "drop table test_free_result");
  myquery(rc);

  mysql_stmt_close(stmt);
}