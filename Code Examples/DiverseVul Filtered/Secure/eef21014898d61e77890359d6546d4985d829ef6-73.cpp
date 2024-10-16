static void test_bug27876()
{
  int rc;
  MYSQL_RES *result;

  uchar utf8_func[] =
  {
    0xd1, 0x84, 0xd1, 0x83, 0xd0, 0xbd, 0xd0, 0xba,
    0xd1, 0x86, 0xd0, 0xb8, 0xd0, 0xb9, 0xd0, 0xba,
    0xd0, 0xb0,
    0x00
  };

  uchar utf8_param[] =
  {
    0xd0, 0xbf, 0xd0, 0xb0, 0xd1, 0x80, 0xd0, 0xb0,
    0xd0, 0xbc, 0xd0, 0xb5, 0xd1, 0x82, 0xd1, 0x8a,
    0xd1, 0x80, 0x5f, 0xd0, 0xb2, 0xd0, 0xb5, 0xd1,
    0x80, 0xd1, 0x81, 0xd0, 0xb8, 0xd1, 0x8f,
    0x00
  };

  char query[500];

  DBUG_ENTER("test_bug27876");
  myheader("test_bug27876");

  rc= mysql_query(mysql, "set names utf8");
  myquery(rc);

  rc= mysql_query(mysql, "select version()");
  myquery(rc);
  result= mysql_store_result(mysql);
  mytest(result);
  mysql_free_result(result);

  sprintf(query, "DROP FUNCTION IF EXISTS %s", (char*) utf8_func);
  rc= mysql_query(mysql, query);
  myquery(rc);

  sprintf(query,
          "CREATE FUNCTION %s( %s VARCHAR(25))"
          " RETURNS VARCHAR(25) DETERMINISTIC RETURN %s",
          (char*) utf8_func, (char*) utf8_param, (char*) utf8_param);
  rc= mysql_query(mysql, query);
  myquery(rc);
  sprintf(query, "SELECT %s(VERSION())", (char*) utf8_func);
  rc= mysql_query(mysql, query);
  myquery(rc);
  result= mysql_store_result(mysql);
  mytest(result);
  mysql_free_result(result);

  sprintf(query, "DROP FUNCTION %s", (char*) utf8_func);
  rc= mysql_query(mysql, query);
  myquery(rc);

  rc= mysql_query(mysql, "set names default");
  myquery(rc);

  DBUG_VOID_RETURN;
}