static void test_date()
{
  int        rc;

  myheader("test_date");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 TIMESTAMP, \
                                                 c2 TIME, \
                                                 c3 DATETIME, \
                                                 c4 DATE)");

  myquery(rc);

  test_bind_date_conv(5);
}