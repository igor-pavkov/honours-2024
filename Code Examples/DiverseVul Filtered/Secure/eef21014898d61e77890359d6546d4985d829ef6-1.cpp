static void test_bug4030()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[3];
  MYSQL_TIME time_canonical, time_out;
  MYSQL_TIME date_canonical, date_out;
  MYSQL_TIME datetime_canonical, datetime_out;
  const char *stmt_text;
  int rc;

  myheader("test_bug4030");

  /* Check that microseconds are inserted and selected successfully */

  /* Execute a query with time values in prepared mode */
  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT '23:59:59.123456', '2003-12-31', "
             "'2003-12-31 23:59:59.123456'";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* Bind output buffers */
  bzero((char*) my_bind, sizeof(my_bind));
  bzero((char*) &time_canonical, sizeof(time_canonical));
  bzero((char*) &time_out, sizeof(time_out));
  bzero((char*) &date_canonical, sizeof(date_canonical));
  bzero((char*) &date_out, sizeof(date_out));
  bzero((char*) &datetime_canonical, sizeof(datetime_canonical));
  bzero((char*) &datetime_out, sizeof(datetime_out));

  my_bind[0].buffer_type= MYSQL_TYPE_TIME;
  my_bind[0].buffer= (void *) &time_out;
  my_bind[1].buffer_type= MYSQL_TYPE_DATE;
  my_bind[1].buffer= (void *) &date_out;
  my_bind[2].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[2].buffer= (void *) &datetime_out;

  time_canonical.hour= 23;
  time_canonical.minute= 59;
  time_canonical.second= 59;
  time_canonical.second_part= 123456;
  time_canonical.time_type= MYSQL_TIMESTAMP_TIME;

  date_canonical.year= 2003;
  date_canonical.month= 12;
  date_canonical.day= 31;
  date_canonical.time_type= MYSQL_TIMESTAMP_DATE;

  datetime_canonical= time_canonical;
  datetime_canonical.year= 2003;
  datetime_canonical.month= 12;
  datetime_canonical.day= 31;
  datetime_canonical.time_type= MYSQL_TIMESTAMP_DATETIME;

  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);
  if (!opt_silent)
  {
    printf("%d:%d:%d.%lu\n", time_out.hour, time_out.minute, time_out.second,
           time_out.second_part);
    printf("%d-%d-%d\n", date_out.year, date_out.month, date_out.day);
    printf("%d-%d-%d %d:%d:%d.%lu\n", datetime_out.year, datetime_out.month,
           datetime_out.day, datetime_out.hour,
           datetime_out.minute, datetime_out.second,
           datetime_out.second_part);
  }
  DIE_UNLESS(memcmp(&time_canonical, &time_out, sizeof(time_out)) == 0);
  DIE_UNLESS(memcmp(&date_canonical, &date_out, sizeof(date_out)) == 0);
  DIE_UNLESS(memcmp(&datetime_canonical, &datetime_out, sizeof(datetime_out)) == 0);
  mysql_stmt_close(stmt);
}