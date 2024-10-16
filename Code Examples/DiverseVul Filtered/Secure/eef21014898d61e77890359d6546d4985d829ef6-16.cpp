static void test_progress_reporting()
{
  int rc, i;
  MYSQL*       conn;

  /* Progress reporting doesn't work yet with embedded server */
  if (embedded_server_arg_count)
    return;

  myheader("test_progress_reporting");

  conn= client_connect(CLIENT_PROGRESS, MYSQL_PROTOCOL_TCP, 0);
  DIE_UNLESS(conn->client_flag & CLIENT_PROGRESS);

  mysql_options(conn, MYSQL_PROGRESS_CALLBACK, (void*) report_progress);
  rc= mysql_query(conn, "set @save=@@global.progress_report_time");
  myquery(rc);
  rc= mysql_query(conn, "set @@global.progress_report_time=1");
  myquery(rc);

  rc= mysql_query(conn, "drop table if exists t1,t2");
  myquery(rc);
  rc= mysql_query(conn, "create table t1 (f2 varchar(255)) engine=aria");
  myquery(rc);
  rc= mysql_query(conn, "create table t2 like t1");
  myquery(rc);
  rc= mysql_query(conn, "insert into t1 (f2) values (repeat('a',100)),(repeat('b',200)),(repeat('c',202)),(repeat('d',202)),(repeat('e',202)),(repeat('f',202)),(repeat('g',23))");
  myquery(rc);
  for (i= 0 ; i < 5 ; i++)
  {
    rc= mysql_query(conn, "insert into t2 (f2) select f2 from t1");
    myquery(rc);
    rc= mysql_query(conn, "insert into t1 (f2) select f2 from t2");
    myquery(rc);
  }
  rc= mysql_query(conn, "alter table t1 add f1 int primary key auto_increment, add key (f2), order by f2");
  myquery(rc);
  if (!opt_silent)
    printf("Got progress_count: %u  stage: %u  max_stage: %u\n",
           progress_count, progress_stage, progress_max_stage);
  DIE_UNLESS(progress_count > 0 && progress_stage >=2 && progress_max_stage == 3);
  myquery(rc);
  rc= mysql_query(conn, "set @@global.progress_report_time=@save");
  myquery(rc);
  mysql_close(conn);
}