static void test_wl4166_4()
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

  myheader("test_wl4166_4");

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  /*
    Create table with binary columns, set session character set to cp1251,
    client character set to koi8, and make sure that there is conversion
    on insert and no conversion on select
  */
  rc= mysql_query(mysql,
                  "create table t1 (c1 varbinary(255), c2 varbinary(255))");
  myquery(rc);
  rc= mysql_query(mysql, "set character_set_client=koi8r, "
                         "character_set_connection=cp1251, "
                         "character_set_results=koi8r");
  myquery(rc);

  bzero((char*) bind_array, sizeof(bind_array));

  bind_array[0].buffer_type= MYSQL_TYPE_STRING;

  bind_array[1].buffer_type= MYSQL_TYPE_STRING;
  bind_array[1].buffer= (void *) koi8;
  bind_array[1].buffer_length= strlen(koi8);

  stmt= mysql_stmt_init(mysql);
  check_stmt(stmt);

  stmt_text= "insert into t1 (c1, c2) values (?, ?)";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  mysql_stmt_bind_param(stmt, bind_array);

  mysql_stmt_send_long_data(stmt, 0, koi8, strlen(koi8));

  /* Cause a reprepare at statement execute */
  rc= mysql_query(mysql, "alter table t1 add column d int");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  stmt_text= "select c1, c2 from t1";

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

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
  rc= mysql_query(mysql, "set names default");
  myquery(rc);
}