static void test_bug4172()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[3];
  const char *stmt_text;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int rc;
  char f[100], d[100], e[100];
  ulong f_len, d_len, e_len;

  myheader("test_bug4172");

  mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  mysql_query(mysql, "CREATE TABLE t1 (f float, d double, e decimal(10,4))");
  mysql_query(mysql, "INSERT INTO t1 VALUES (12345.1234, 123456.123456, "
                                            "123456.1234)");

  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT f, d, e FROM t1";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= f;
  my_bind[0].buffer_length= sizeof(f);
  my_bind[0].length= &f_len;
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= d;
  my_bind[1].buffer_length= sizeof(d);
  my_bind[1].length= &d_len;
  my_bind[2].buffer_type= MYSQL_TYPE_STRING;
  my_bind[2].buffer= e;
  my_bind[2].buffer_length= sizeof(e);
  my_bind[2].length= &e_len;

  mysql_stmt_bind_result(stmt, my_bind);

  mysql_stmt_store_result(stmt);
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  res= mysql_store_result(mysql);
  row= mysql_fetch_row(res);

  if (!opt_silent)
  {
    printf("Binary protocol: float=%s, double=%s, decimal(10,4)=%s\n",
           f, d, e);
    printf("Text protocol:   float=%s, double=%s, decimal(10,4)=%s\n",
           row[0], row[1], row[2]);
  }
  DIE_UNLESS(!strcmp(f, row[0]) && !strcmp(d, row[1]) && !strcmp(e, row[2]));

  mysql_free_result(res);
  mysql_stmt_close(stmt);
}