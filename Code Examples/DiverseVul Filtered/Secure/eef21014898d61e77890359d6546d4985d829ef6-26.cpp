static void test_bug4026()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  MYSQL_TIME time_in, time_out;
  MYSQL_TIME datetime_in, datetime_out;
  const char *stmt_text;
  int rc;

  myheader("test_bug4026");

  /* Check that microseconds are inserted and selected successfully */

  /* Create a statement handle and prepare it with select */
  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT ?, ?";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  /* Bind input buffers */
  bzero((char*) my_bind, sizeof(my_bind));
  bzero((char*) &time_in, sizeof(time_in));
  bzero((char*) &time_out, sizeof(time_out));
  bzero((char*) &datetime_in, sizeof(datetime_in));
  bzero((char*) &datetime_out, sizeof(datetime_out));

  my_bind[0].buffer_type= MYSQL_TYPE_TIME;
  my_bind[0].buffer= (void *) &time_in;
  my_bind[1].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[1].buffer= (void *) &datetime_in;

  time_in.hour= 23;
  time_in.minute= 59;
  time_in.second= 59;
  time_in.second_part= 123456;
  /*
    This is not necessary, just to make DIE_UNLESS below work: this field
    is filled in when time is received from server
  */
  time_in.time_type= MYSQL_TIMESTAMP_TIME;

  datetime_in= time_in;
  datetime_in.year= 2003;
  datetime_in.month= 12;
  datetime_in.day= 31;
  datetime_in.time_type= MYSQL_TIMESTAMP_DATETIME;

  mysql_stmt_bind_param(stmt, my_bind);

  /* Execute the select statement */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  my_bind[0].buffer= (void *) &time_out;
  my_bind[1].buffer= (void *) &datetime_out;

  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);
  if (!opt_silent)
  {
    printf("%d:%d:%d.%lu\n", time_out.hour, time_out.minute, time_out.second,
           time_out.second_part);
    printf("%d-%d-%d %d:%d:%d.%lu\n", datetime_out.year, datetime_out.month,
           datetime_out.day, datetime_out.hour,
           datetime_out.minute, datetime_out.second,
           datetime_out.second_part);
  }
  DIE_UNLESS(memcmp(&time_in, &time_out, sizeof(time_in)) == 0);
  DIE_UNLESS(memcmp(&datetime_in, &datetime_out, sizeof(datetime_in)) == 0);
  mysql_stmt_close(stmt);
}