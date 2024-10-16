static void test_wl4166_3()
{
  int rc;
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  MYSQL_TIME tm[1];

  myheader("test_wl4166_3");

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1 (year datetime)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "insert into t1 (year) values (?)");
  check_stmt(stmt);
  verify_param_count(stmt, 1);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[0].buffer= &tm[0];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  tm[0].year= 10000;
  tm[0].month= 1; tm[0].day= 1;
  tm[0].hour= 1; tm[0].minute= 1; tm[0].second= 1;
  tm[0].second_part= 0; tm[0].neg= 0;

  /* Cause a statement reprepare */
  rc= mysql_query(mysql, "alter table t1 add column c int");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  /*
    Sic: only one warning, instead of two. The warning
    about data truncation when assigning a parameter is lost.
    This is a bug.
  */
  my_process_warnings(mysql, 1);

  verify_col_data("t1", "year", "0000-00-00 00:00:00");

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}