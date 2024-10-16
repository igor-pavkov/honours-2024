static void test_date_dt()
{
  int rc;

  myheader("test_date_dt");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 datetime, "
                         " c2 datetime, c3 datetime, c4 date)");
  myquery(rc);

  test_bind_date_conv(2);
}