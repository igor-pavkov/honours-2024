static void test_bug49972()
{
  int rc;
  MYSQL_STMT *stmt;

  MYSQL_BIND in_param_bind;
  MYSQL_BIND out_param_bind;
  int int_data;
  my_bool is_null;

  DBUG_ENTER("test_bug49972");
  myheader("test_bug49972");

  rc= mysql_query(mysql, "DROP FUNCTION IF EXISTS f1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP PROCEDURE IF EXISTS p1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE FUNCTION f1() RETURNS INT RETURN 1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE PROCEDURE p1(IN a INT, OUT b INT) SET b = a");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "CALL p1((SELECT f1()), ?)");
  check_stmt(stmt);

  bzero((char *) &in_param_bind, sizeof (in_param_bind));

  in_param_bind.buffer_type= MYSQL_TYPE_LONG;
  in_param_bind.buffer= (char *) &int_data;
  in_param_bind.length= 0;
  in_param_bind.is_null= 0;

  rc= mysql_stmt_bind_param(stmt, &in_param_bind);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  {
    bzero(&out_param_bind, sizeof (out_param_bind));

    out_param_bind.buffer_type= MYSQL_TYPE_LONG;
    out_param_bind.is_null= &is_null;
    out_param_bind.buffer= &int_data;
    out_param_bind.buffer_length= sizeof (int_data);

    rc= mysql_stmt_bind_result(stmt, &out_param_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_fetch(stmt);
    rc= mysql_stmt_fetch(stmt);
    DBUG_ASSERT(rc == MYSQL_NO_DATA);

    mysql_stmt_next_result(stmt);
    mysql_stmt_fetch(stmt);
  }

  rc= mysql_query(mysql, "DROP FUNCTION f1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE FUNCTION f1() RETURNS INT RETURN 1");
  myquery(rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  {
    bzero(&out_param_bind, sizeof (out_param_bind));

    out_param_bind.buffer_type= MYSQL_TYPE_LONG;
    out_param_bind.is_null= &is_null;
    out_param_bind.buffer= &int_data;
    out_param_bind.buffer_length= sizeof (int_data);

    rc= mysql_stmt_bind_result(stmt, &out_param_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_fetch(stmt);
    rc= mysql_stmt_fetch(stmt);
    DBUG_ASSERT(rc == MYSQL_NO_DATA);

    mysql_stmt_next_result(stmt);
    mysql_stmt_fetch(stmt);
  }

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP PROCEDURE p1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP FUNCTION f1");
  myquery(rc);

  DBUG_VOID_RETURN;
}