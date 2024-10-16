static void test_fetch_date()
{
  MYSQL_STMT *stmt;
  uint       i;
  int        rc, year;
  char       date[25], my_time[25], ts[25], ts_4[25], ts_6[20], dt[20];
  ulong      d_length, t_length, ts_length, ts4_length, ts6_length,
             dt_length, y_length;
  MYSQL_BIND my_bind[8];
  my_bool    is_null[8];
  ulong      length[8];

  myheader("test_fetch_date");

  /* Will not work if sql_mode is NO_ZERO_DATE (implicit if TRADITIONAL) */
  rc= mysql_query(mysql, "SET SQL_MODE=''");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_bind_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bind_result(c1 date, c2 time, \
                                                        c3 timestamp, \
                                                        c4 year, \
                                                        c5 datetime, \
                                                        c6 timestamp, \
                                                        c7 timestamp)");
  myquery(rc);

  rc= mysql_query(mysql, "SET SQL_MODE=''");
  rc= mysql_query(mysql, "INSERT INTO test_bind_result VALUES('2002-01-02', \
                                                              '12:49:00', \
                                                              '2002-01-02 17:46:59', \
                                                              2010, \
                                                              '2010-07-10', \
                                                              '2020', '1999-12-29')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  bzero((char*) my_bind, sizeof(my_bind));
  for (i= 0; i < array_elements(my_bind); i++)
  {
    my_bind[i].is_null= &is_null[i];
    my_bind[i].length= &length[i];
  }

  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1]= my_bind[2]= my_bind[0];

  my_bind[0].buffer= (void *)&date;
  my_bind[0].buffer_length= sizeof(date);
  my_bind[0].length= &d_length;

  my_bind[1].buffer= (void *)&my_time;
  my_bind[1].buffer_length= sizeof(my_time);
  my_bind[1].length= &t_length;

  my_bind[2].buffer= (void *)&ts;
  my_bind[2].buffer_length= sizeof(ts);
  my_bind[2].length= &ts_length;

  my_bind[3].buffer_type= MYSQL_TYPE_LONG;
  my_bind[3].buffer= (void *)&year;
  my_bind[3].length= &y_length;

  my_bind[4].buffer_type= MYSQL_TYPE_STRING;
  my_bind[4].buffer= (void *)&dt;
  my_bind[4].buffer_length= sizeof(dt);
  my_bind[4].length= &dt_length;

  my_bind[5].buffer_type= MYSQL_TYPE_STRING;
  my_bind[5].buffer= (void *)&ts_4;
  my_bind[5].buffer_length= sizeof(ts_4);
  my_bind[5].length= &ts4_length;

  my_bind[6].buffer_type= MYSQL_TYPE_STRING;
  my_bind[6].buffer= (void *)&ts_6;
  my_bind[6].buffer_length= sizeof(ts_6);
  my_bind[6].length= &ts6_length;

  rc= my_stmt_result("SELECT * FROM test_bind_result");
  DIE_UNLESS(rc == 1);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_bind_result");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  ts_4[0]= '\0';
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "\n date   : %s(%lu)", date, d_length);
    fprintf(stdout, "\n time   : %s(%lu)", my_time, t_length);
    fprintf(stdout, "\n ts     : %s(%lu)", ts, ts_length);
    fprintf(stdout, "\n year   : %d(%lu)", year, y_length);
    fprintf(stdout, "\n dt     : %s(%lu)", dt,  dt_length);
    fprintf(stdout, "\n ts(4)  : %s(%lu)", ts_4, ts4_length);
    fprintf(stdout, "\n ts(6)  : %s(%lu)", ts_6, ts6_length);
  }

  DIE_UNLESS(strcmp(date, "2002-01-02") == 0);
  DIE_UNLESS(d_length == 10);

  DIE_UNLESS(strcmp(my_time, "12:49:00") == 0);
  DIE_UNLESS(t_length == 8);

  DIE_UNLESS(strcmp(ts, "2002-01-02 17:46:59") == 0);
  DIE_UNLESS(ts_length == 19);

  DIE_UNLESS(year == 2010);
  DIE_UNLESS(y_length == 4);

  DIE_UNLESS(strcmp(dt, "2010-07-10 00:00:00") == 0);
  DIE_UNLESS(dt_length == 19);

  DIE_UNLESS(strcmp(ts_4, "0000-00-00 00:00:00") == 0);
  DIE_UNLESS(ts4_length == strlen("0000-00-00 00:00:00"));

  DIE_UNLESS(strcmp(ts_6, "1999-12-29 00:00:00") == 0);
  DIE_UNLESS(ts6_length == 19);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}