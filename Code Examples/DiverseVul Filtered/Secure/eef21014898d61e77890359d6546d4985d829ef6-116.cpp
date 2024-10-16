static void test_date_ts()
{
  int        rc;

  myheader("test_date_ts");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 TIMESTAMP, \
                                                 c2 TIMESTAMP, \
                                                 c3 TIMESTAMP, \
                                                 c4 TIMESTAMP)");

  myquery(rc);

  test_bind_date_conv(2);
}