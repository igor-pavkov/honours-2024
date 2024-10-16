static void test_ps_sp_out_params()
{
  MYSQL *my;
  MYSQL_STMT *stmt;
  MYSQL_BIND bind[1];
  char buffer[20];
  int status, rc;

  myheader("test_ps_sp_out_params");
  my= mysql_client_init(NULL);

  if (!mysql_real_connect(my, opt_host, opt_user,
                               opt_password, current_db, opt_port,
                               opt_unix_socket, CLIENT_MULTI_RESULTS))
    DIE("mysql_real_connect failed");

  rc= mysql_query(my, "DROP PROCEDURE IF EXISTS p1");
  myquery(rc);

  rc= mysql_query(my,
    "CREATE PROCEDURE p1(OUT out_param VARCHAR(19)) "
    "BEGIN"
    " SELECT 'foo' FROM DUAL;"
    " SET out_param='foo';"
    " SELECT 'foo' FROM DUAL;"
    "END");
  myquery(rc);

  stmt= mysql_stmt_init(my);

  rc= mysql_stmt_prepare(stmt, "CALL P1(?)", 10);
  DIE_UNLESS(rc==0);

  DIE_UNLESS(mysql_stmt_param_count(stmt) == 1);

  memset(bind, 0, sizeof(MYSQL_BIND));
  bind[0].buffer= buffer;
  bind[0].buffer_length= sizeof(buffer);
  bind[0].buffer_type= MYSQL_TYPE_STRING;

  mysql_stmt_bind_param(stmt, bind);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  do {
    if (mysql_stmt_field_count(stmt))
    {
      /* since server sends a status packet at the end,
         there must follow at least one additional packet */
      DIE_UNLESS(mysql_more_results(stmt->mysql));

      mysql_stmt_bind_result(stmt, bind);

      rc= mysql_stmt_fetch(stmt);
      DIE_UNLESS(rc== 0);

      DIE_UNLESS(strcmp(buffer, "foo") == 0);
    }
    status= mysql_stmt_next_result(stmt);
  } while (status == 0);

  rc= mysql_stmt_reset(stmt);
  DIE_UNLESS(rc== 0);

  mysql_stmt_close(stmt);
  mysql_close(my);

  printf("end\n");
}