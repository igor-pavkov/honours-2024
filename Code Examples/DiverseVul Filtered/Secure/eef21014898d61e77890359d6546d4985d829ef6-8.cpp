static void test_fetch_column()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  char       c2[20], bc2[20];
  ulong      l1, l2, bl1, bl2;
  int        rc, c1, bc1;

  myheader("test_fetch_column");

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1(c1 int primary key auto_increment, c2 char(10))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1(c2) values('venu'), ('mysql')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select * from t1 order by c2 desc");
  check_stmt(stmt);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&bc1;
  my_bind[0].buffer_length= 0;
  my_bind[0].is_null= 0;
  my_bind[0].length= &bl1;
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)bc2;
  my_bind[1].buffer_length= 7;
  my_bind[1].is_null= 0;
  my_bind[1].length= &bl2;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0); /* No-op at this point */
  check_execute_r(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 0: %d, %s", bc1, bc2);

  c2[0]= '\0'; l2= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)c2;
  my_bind[0].buffer_length= 7;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l2;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s(%ld)", c2, l2);
  DIE_UNLESS(strncmp(c2, "venu", 4) == 0 && l2 == 4);

  c2[0]= '\0'; l2= 0;
  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s(%ld)", c2, l2);
  DIE_UNLESS(strcmp(c2, "venu") == 0 && l2 == 4);

  c1= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&c1;
  my_bind[0].buffer_length= 0;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l1;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 0: %d(%ld)", c1, l1);
  DIE_UNLESS(c1 == 1 && l1 == 4);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d, %s", bc1, bc2);

  c2[0]= '\0'; l2= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)c2;
  my_bind[0].buffer_length= 7;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l2;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %s(%ld)", c2, l2);
  DIE_UNLESS(strncmp(c2, "mysq", 4) == 0 && l2 == 5);

  c2[0]= '\0'; l2= 0;
  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 1: %si(%ld)", c2, l2);
  DIE_UNLESS(strcmp(c2, "mysql") == 0 && l2 == 5);

  c1= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&c1;
  my_bind[0].buffer_length= 0;
  my_bind[0].is_null= 0;
  my_bind[0].length= &l1;

  rc= mysql_stmt_fetch_column(stmt, my_bind, 0, 0);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n col 0: %d(%ld)", c1, l1);
  DIE_UNLESS(c1 == 2 && l1 == 4);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  rc= mysql_stmt_fetch_column(stmt, my_bind, 1, 0);
  check_execute_r(stmt, rc);

  mysql_stmt_close(stmt);
  myquery(mysql_query(mysql, "drop table t1"));
}