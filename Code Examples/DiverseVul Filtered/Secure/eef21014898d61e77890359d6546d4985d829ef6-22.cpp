static void test_conversion()
{
  MYSQL_STMT *stmt;
  const char *stmt_text;
  int rc;
  MYSQL_BIND my_bind[1];
  uchar buff[4];
  ulong length;

  myheader("test_conversion");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "CREATE TABLE t1 (a TEXT) DEFAULT CHARSET latin1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "SET character_set_connection=utf8, character_set_client=utf8, "
             " character_set_results=latin1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_stmt_init(mysql);

  stmt_text= "INSERT INTO t1 (a) VALUES (?)";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer= (char*) buff;
  my_bind[0].length= &length;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;

  mysql_stmt_bind_param(stmt, my_bind);

  buff[0]= (uchar) 0xC3;
  buff[1]= (uchar) 0xA0;
  length= 2;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  stmt_text= "SELECT a FROM t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  my_bind[0].buffer_length= sizeof(buff);
  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);
  DIE_UNLESS(length == 1);
  DIE_UNLESS(buff[0] == 0xE0);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
  stmt_text= "DROP TABLE t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "SET NAMES DEFAULT";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}