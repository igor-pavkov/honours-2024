static void test_bug40365(void)
{
  uint         rc, i;
  MYSQL_STMT   *stmt= 0;
  MYSQL_BIND   my_bind[2];
  my_bool      is_null[2]= {0};
  MYSQL_TIME   tm[2];

  DBUG_ENTER("test_bug40365");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);
  rc= mysql_query(mysql, "CREATE TABLE t1(c1 DATETIME, \
                                          c2 DATE)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "INSERT INTO t1 VALUES(?, ?)");
  check_stmt(stmt);
  verify_param_count(stmt, 2);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[1].buffer_type= MYSQL_TYPE_DATE;
  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].buffer= (void *) &tm[i];
    my_bind[i].is_null= &is_null[i];
  }

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    tm[i].neg= 0;
    tm[i].second_part= 0;
    tm[i].year= 2009;
    tm[i].month= 2;
    tm[i].day= 29;
    tm[i].hour= 0;
    tm[i].minute= 0;
    tm[i].second= 0;
  }
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_commit(mysql);
  myquery(rc);
  mysql_stmt_close(stmt);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM t1");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n");

  for (i= 0; i < array_elements(my_bind); i++)
  {
    if (!opt_silent)
      fprintf(stdout, "\ntime[%d]: %02d-%02d-%02d ",
              i, tm[i].year, tm[i].month, tm[i].day);
      DIE_UNLESS(tm[i].year == 0);
      DIE_UNLESS(tm[i].month == 0);
      DIE_UNLESS(tm[i].day == 0);
  }
  mysql_stmt_close(stmt);
  rc= mysql_commit(mysql);
  myquery(rc);

  DBUG_VOID_RETURN;
}