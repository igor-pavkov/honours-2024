static void test_ps_i18n()
{
  MYSQL_STMT *stmt;
  int rc;
  const char *stmt_text;
  MYSQL_BIND bind_array[2];

  /* Represented as numbers to keep UTF8 tools from clobbering them. */
  const char *koi8= "\xee\xd5\x2c\x20\xda\xc1\x20\xd2\xd9\xc2\xc1\xcc\xcb\xd5";
  const char *cp1251= "\xcd\xf3\x2c\x20\xe7\xe0\x20\xf0\xfb\xe1\xe0\xeb\xea\xf3";
  char buf1[16], buf2[16];
  ulong buf1_len, buf2_len;


  myheader("test_ps_i18n");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  /*
    Create table with binary columns, set session character set to cp1251,
    client character set to koi8, and make sure that there is conversion
    on insert and no conversion on select
  */

  stmt_text= "CREATE TABLE t1 (c1 VARBINARY(255), c2 VARBINARY(255))";

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "SET CHARACTER_SET_CLIENT=koi8r, "
                 "CHARACTER_SET_CONNECTION=cp1251, "
                 "CHARACTER_SET_RESULTS=koi8r";

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  bzero((char*) bind_array, sizeof(bind_array));

  bind_array[0].buffer_type= MYSQL_TYPE_STRING;
  bind_array[0].buffer= (void *) koi8;
  bind_array[0].buffer_length= strlen(koi8);

  bind_array[1].buffer_type= MYSQL_TYPE_STRING;
  bind_array[1].buffer= (void *) koi8;
  bind_array[1].buffer_length= strlen(koi8);

  stmt= mysql_stmt_init(mysql);
  check_stmt(stmt);

  stmt_text= "INSERT INTO t1 (c1, c2) VALUES (?, ?)";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  mysql_stmt_bind_param(stmt, bind_array);

  mysql_stmt_send_long_data(stmt, 0, koi8, strlen(koi8));

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  stmt_text= "SELECT c1, c2 FROM t1";

  /* c1 and c2 are binary so no conversion will be done on select */
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bind_array[0].buffer= buf1;
  bind_array[0].buffer_length= sizeof(buf1);
  bind_array[0].length= &buf1_len;

  bind_array[1].buffer= buf2;
  bind_array[1].buffer_length= sizeof(buf2);
  bind_array[1].length= &buf2_len;

  mysql_stmt_bind_result(stmt, bind_array);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(buf1_len == strlen(cp1251));
  DIE_UNLESS(buf2_len == strlen(cp1251));
  DIE_UNLESS(!memcmp(buf1, cp1251, buf1_len));
  DIE_UNLESS(!memcmp(buf2, cp1251, buf1_len));

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  /*
    Now create table with two cp1251 columns, set client character
    set to koi8 and supply columns of one row as string and another as
    binary data. Binary data must not be converted on insert, and both
    columns must be converted to client character set on select.
  */

  stmt_text= "CREATE TABLE t1 (c1 VARCHAR(255) CHARACTER SET cp1251, "
                              "c2 VARCHAR(255) CHARACTER SET cp1251)";

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "INSERT INTO t1 (c1, c2) VALUES (?, ?)";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  /* this data must be converted */
  bind_array[0].buffer_type= MYSQL_TYPE_STRING;
  bind_array[0].buffer= (void *) koi8;
  bind_array[0].buffer_length= strlen(koi8);

  bind_array[1].buffer_type= MYSQL_TYPE_STRING;
  bind_array[1].buffer= (void *) koi8;
  bind_array[1].buffer_length= strlen(koi8);

  mysql_stmt_bind_param(stmt, bind_array);

  mysql_stmt_send_long_data(stmt, 0, koi8, strlen(koi8));

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* this data must not be converted */
  bind_array[0].buffer_type= MYSQL_TYPE_BLOB;
  bind_array[0].buffer= (void *) cp1251;
  bind_array[0].buffer_length= strlen(cp1251);

  bind_array[1].buffer_type= MYSQL_TYPE_BLOB;
  bind_array[1].buffer= (void *) cp1251;
  bind_array[1].buffer_length= strlen(cp1251);

  mysql_stmt_bind_param(stmt, bind_array);

  mysql_stmt_send_long_data(stmt, 0, cp1251, strlen(cp1251));

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  /* Fetch data and verify that rows are in koi8 */

  stmt_text= "SELECT c1, c2 FROM t1";

  /* c1 and c2 are binary so no conversion will be done on select */
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bind_array[0].buffer= buf1;
  bind_array[0].buffer_length= sizeof(buf1);
  bind_array[0].length= &buf1_len;

  bind_array[1].buffer= buf2;
  bind_array[1].buffer_length= sizeof(buf2);
  bind_array[1].length= &buf2_len;

  mysql_stmt_bind_result(stmt, bind_array);

  while ((rc= mysql_stmt_fetch(stmt)) == 0)
  {
    DIE_UNLESS(buf1_len == strlen(koi8));
    DIE_UNLESS(buf2_len == strlen(koi8));
    DIE_UNLESS(!memcmp(buf1, koi8, buf1_len));
    DIE_UNLESS(!memcmp(buf2, koi8, buf1_len));
  }
  DIE_UNLESS(rc == MYSQL_NO_DATA);
  mysql_stmt_close(stmt);

  stmt_text= "DROP TABLE t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "SET NAMES DEFAULT";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}