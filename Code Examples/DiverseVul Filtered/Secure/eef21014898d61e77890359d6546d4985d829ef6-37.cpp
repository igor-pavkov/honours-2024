static void test_multi()
{
  MYSQL_STMT *stmt_delete, *stmt_update, *stmt_select1, *stmt_select2;
  char *query;
  MYSQL_BIND my_bind[1];
  int rc, i;
  int32 param= 1;
  ulong length= 1;
  myheader("test_multi");

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&param;
  my_bind[0].length= &length;

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1, t2");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1 (a int, b int)");
  myquery(rc);

  rc= mysql_query(mysql, "create table t2 (a int, b int)");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1 values (3, 3), (2, 2), (1, 1)");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t2 values (3, 3), (2, 2), (1, 1)");
  myquery(rc);

  query= (char*)"delete t1, t2 from t1, t2 where t1.a=t2.a and t1.b=10";
  stmt_delete= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_delete);

  query= (char*)"update t1, t2 set t1.b=10, t2.b=10 where t1.a=t2.a and t1.b=?";
  stmt_update= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_update);

  query= (char*)"select * from t1";
  stmt_select1= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_select1);

  query= (char*)"select * from t2";
  stmt_select2= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_select2);

  for(i= 0; i < 3; i++)
  {
    rc= mysql_stmt_bind_param(stmt_update, my_bind);
    check_execute(stmt_update, rc);

    rc= mysql_stmt_execute(stmt_update);
    check_execute(stmt_update, rc);
    if (!opt_silent)
      fprintf(stdout, "update %ld\n", (long) param);

    rc= mysql_stmt_execute(stmt_delete);
    check_execute(stmt_delete, rc);
    if (!opt_silent)
      fprintf(stdout, "delete %ld\n", (long) param);

    rc= mysql_stmt_execute(stmt_select1);
    check_execute(stmt_select1, rc);
    rc= my_process_stmt_result(stmt_select1);
    DIE_UNLESS(rc == 3-param);

    rc= mysql_stmt_execute(stmt_select2);
    check_execute(stmt_select2, rc);
    rc= my_process_stmt_result(stmt_select2);
    DIE_UNLESS(rc == 3-param);

    param++;
  }

  mysql_stmt_close(stmt_delete);
  mysql_stmt_close(stmt_update);
  mysql_stmt_close(stmt_select1);
  mysql_stmt_close(stmt_select2);
  rc= mysql_query(mysql, "drop table t1, t2");
  myquery(rc);
}