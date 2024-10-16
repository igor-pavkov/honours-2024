static void test_bug4231()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  MYSQL_TIME tm[2];
  const char *stmt_text;
  int rc;

  myheader("test_bug4231");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "CREATE TABLE t1 (a int)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "INSERT INTO t1 VALUES (1)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT a FROM t1 WHERE ? = ?";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  /* Bind input buffers */
  bzero((char*) my_bind, sizeof(my_bind));
  bzero((char*) tm, sizeof(tm));

  my_bind[0].buffer_type= MYSQL_TYPE_DATE;
  my_bind[0].buffer= &tm[0];
  my_bind[1].buffer_type= MYSQL_TYPE_DATE;
  my_bind[1].buffer= &tm[1];

  mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  /*
    First set server-side params to some non-zero non-equal values:
    then we will check that they are not used when client sends
    new (zero) times.
  */
  tm[0].time_type = MYSQL_TIMESTAMP_DATE;
  tm[0].year = 2000;
  tm[0].month = 1;
  tm[0].day = 1;
  tm[1]= tm[0];
  --tm[1].year;                                 /* tm[0] != tm[1] */

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);

  /* binds are unequal, no rows should be returned */
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* Set one of the dates to zero */
  tm[0].year= tm[0].month= tm[0].day= 0;
  tm[1]= tm[0];
  mysql_stmt_execute(stmt);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);

  mysql_stmt_close(stmt);
  stmt_text= "DROP TABLE t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}