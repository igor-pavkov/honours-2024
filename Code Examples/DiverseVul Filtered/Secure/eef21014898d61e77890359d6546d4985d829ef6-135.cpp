static void test_date_time()
{
  int        rc;

  myheader("test_date_time");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 TIME, \
                                                 c2 TIME, \
                                                 c3 TIME, \
                                                 c4 TIME)");

  myquery(rc);

  test_bind_date_conv(3);
}