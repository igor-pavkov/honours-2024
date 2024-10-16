static void test_date_date()
{
  int        rc;

  myheader("test_date_date");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 DATE, \
                                                 c2 DATE, \
                                                 c3 DATE, \
                                                 c4 DATE)");

  myquery(rc);

  test_bind_date_conv(3);
}