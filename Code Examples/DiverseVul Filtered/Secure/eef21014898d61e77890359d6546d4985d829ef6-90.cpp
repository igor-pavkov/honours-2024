static void test_bug41078(void)
{
  uint         rc;
  MYSQL_STMT   *stmt= 0;
  MYSQL_BIND   param, result;
  ulong        cursor_type= CURSOR_TYPE_READ_ONLY;
  ulong        len;
  char         str[64];
  const char   param_str[]= "abcdefghijklmn";
  my_bool      is_null, error;

  DBUG_ENTER("test_bug41078");

  rc= mysql_query(mysql, "SET NAMES UTF8");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "SELECT ?");
  check_stmt(stmt);
  verify_param_count(stmt, 1);

  rc= mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, &cursor_type);
  check_execute(stmt, rc);
  
  bzero(&param, sizeof(param));
  param.buffer_type= MYSQL_TYPE_STRING;
  param.buffer= (void *) param_str;
  len= sizeof(param_str) - 1;
  param.length= &len;

  rc= mysql_stmt_bind_param(stmt, &param);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero(&result, sizeof(result));
  result.buffer_type= MYSQL_TYPE_STRING;
  result.buffer= str;
  result.buffer_length= sizeof(str);
  result.is_null= &is_null;
  result.length= &len;
  result.error=  &error;
  
  rc= mysql_stmt_bind_result(stmt, &result);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(len == sizeof(param_str) - 1 && !strcmp(str, param_str));

  mysql_stmt_close(stmt);

  DBUG_VOID_RETURN;
}