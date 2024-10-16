static void test_wl4166_2()
{
  MYSQL_STMT *stmt;
  int        c_int;
  MYSQL_TIME d_date;
  MYSQL_BIND bind_out[2];
  int rc;

  myheader("test_wl4166_2");

  rc= mysql_query(mysql, "SET SQL_MODE=''");
  myquery(rc);

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);
  rc= mysql_query(mysql, "create table t1 (c_int int, d_date date)");
  myquery(rc);
  rc= mysql_query(mysql,
                  "insert into t1 (c_int, d_date) values (42, '1948-05-15')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select * from t1");
  check_stmt(stmt);

  bzero(bind_out, sizeof(bind_out));
  bind_out[0].buffer_type= MYSQL_TYPE_LONG;
  bind_out[0].buffer= (void*) &c_int;

  bind_out[1].buffer_type= MYSQL_TYPE_DATE;
  bind_out[1].buffer= (void*) &d_date;

  rc= mysql_stmt_bind_result(stmt, bind_out);
  check_execute(stmt, rc);

  /* int -> varchar transition */

  rc= mysql_query(mysql,
                  "alter table t1 change column c_int c_int varchar(11)");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(c_int == 42);
  DIE_UNLESS(d_date.year == 1948);
  DIE_UNLESS(d_date.month == 5);
  DIE_UNLESS(d_date.day == 15);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* varchar to int retrieval with truncation */

  rc= mysql_query(mysql, "update t1 set c_int='abcde'");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute_r(stmt, rc);

  DIE_UNLESS(c_int == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* alter table and increase the number of columns */
  rc= mysql_query(mysql, "alter table t1 add column d_int int");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute_r(stmt, rc);

  rc= mysql_stmt_reset(stmt);
  check_execute(stmt, rc);

  /* decrease the number of columns */
  rc= mysql_query(mysql, "alter table t1 drop d_date, drop d_int");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute_r(stmt, rc);

  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);

}