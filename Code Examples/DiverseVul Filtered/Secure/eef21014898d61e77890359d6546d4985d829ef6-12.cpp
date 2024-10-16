static void test_mdev4326()
{
  MYSQL_STMT   *stmt;
  MYSQL_BIND    bind;
  char query[]= "SELECT * FROM mysql.user LIMIT ?";
  char str_data[]= "1";
  unsigned long length= 0;
  int int_data= 1;
  int rc, count;
  my_bool is_null= 0;
  my_bool error= 0;
  myheader("test_mdev4326");

  rc= mysql_change_user(mysql, opt_user, opt_password, "mysql");
  myquery(rc);

  rc= mysql_query(mysql, "SET GLOBAL general_log = 1");
  myquery(rc);

  stmt= mysql_stmt_init(mysql);
  check_stmt(stmt);

  /* Numeric parameter test */

  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);
  check_stmt(stmt);
  verify_param_count(stmt, 1);

  memset((char *)&bind, 0, sizeof(bind));
  bind.buffer_type= MYSQL_TYPE_LONG;
  bind.buffer= (char *)&int_data;
  bind.is_null= &is_null;
  bind.length= &length;
  bind.error= &error;

  rc= mysql_stmt_bind_param(stmt, &bind);
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 1);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 1);
  int_data= 0;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 0);
  rc= mysql_stmt_close(stmt);
  check_execute(stmt, rc);

  /* String parameter test */

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);
  check_stmt(stmt);
  verify_param_count(stmt, 1);

  memset((char *)&bind, 0, sizeof(bind));
  bind.buffer_type= MYSQL_TYPE_STRING;
  bind.buffer= (char *)str_data;
  length= bind.buffer_length= sizeof(str_data);
  bind.is_null= &is_null;
  bind.length= &length;
  bind.error= &error;

  rc= mysql_stmt_bind_param(stmt, &bind);
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 1);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 1);
  str_data[0]= '0';
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  count= 0;
  while (!(rc= mysql_stmt_fetch(stmt)))
    count++;
  DIE_UNLESS(count == 0);
  rc= mysql_stmt_close(stmt);
  check_execute(stmt, rc);

  rc= mysql_change_user(mysql, opt_user, opt_password, current_db);
  myquery(rc);
}