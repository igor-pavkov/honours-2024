static void test_bug11111()
{
  MYSQL_STMT    *stmt;
  MYSQL_BIND    my_bind[2];
  char          buf[2][20];
  ulong         len[2];
  int i;
  int rc;
  const char *query= "SELECT DISTINCT f1,ff2 FROM v1";

  myheader("test_bug11111");

  rc= mysql_query(mysql, "drop table if exists t1, t2, v1");
  myquery(rc);
  rc= mysql_query(mysql, "drop view if exists t1, t2, v1");
  myquery(rc);
  rc= mysql_query(mysql, "create table t1 (f1 int, f2 int)");
  myquery(rc);
  rc= mysql_query(mysql, "create table t2 (ff1 int, ff2 int)");
  myquery(rc);
  rc= mysql_query(mysql, "create view v1 as select * from t1, t2 where f1=ff1");
  myquery(rc);
  rc= mysql_query(mysql, "insert into t1 values (1,1), (2,2), (3,3)");
  myquery(rc);
  rc= mysql_query(mysql, "insert into t2 values (1,1), (2,2), (3,3)");
  myquery(rc);

  stmt= mysql_stmt_init(mysql);

  mysql_stmt_prepare(stmt, query, strlen(query));
  mysql_stmt_execute(stmt);

  bzero((char*) my_bind, sizeof(my_bind));
  for (i=0; i < 2; i++)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_STRING;
    my_bind[i].buffer= (uchar* *)&buf[i];
    my_bind[i].buffer_length= 20;
    my_bind[i].length= &len[i];
  }

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  if (!opt_silent)
    printf("return: %s", buf[1]);
  DIE_UNLESS(!strcmp(buf[1],"1"));
  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "drop view v1");
  myquery(rc);
  rc= mysql_query(mysql, "drop table t1, t2");
  myquery(rc);
}