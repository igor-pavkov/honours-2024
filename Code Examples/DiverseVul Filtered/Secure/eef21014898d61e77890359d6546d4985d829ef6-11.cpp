static void test_fetch_seek()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[3];
  MYSQL_ROW_OFFSET row;
  int        rc;
  int32      c1;
  char       c2[11], c3[20];

  myheader("test_fetch_seek");
  rc= mysql_query(mysql, "drop table if exists t1");

  myquery(rc);

  rc= mysql_query(mysql, "create table t1(c1 int primary key auto_increment, c2 char(10), c3 timestamp)");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1(c2) values('venu'), ('mysql'), ('open'), ('source')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select * from t1");
  check_stmt(stmt);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&c1;

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)c2;
  my_bind[1].buffer_length= sizeof(c2);

  my_bind[2]= my_bind[1];
  my_bind[2].buffer= (void *)c3;
  my_bind[2].buffer_length= sizeof(c3);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 0: %ld, %s, %s", (long) c1, c2, c3);

  row= mysql_stmt_row_tell(stmt);

  row= mysql_stmt_row_seek(stmt, row);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 2: %ld, %s, %s", (long) c1, c2, c3);

  row= mysql_stmt_row_seek(stmt, row);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 2: %ld, %s, %s", (long) c1, c2, c3);

  mysql_stmt_data_seek(stmt, 0);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 0: %ld, %s, %s", (long) c1, c2, c3);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
  myquery(mysql_query(mysql, "drop table t1"));
}