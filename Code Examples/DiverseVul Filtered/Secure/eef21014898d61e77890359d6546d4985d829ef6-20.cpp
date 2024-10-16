static void test_set_variable()
{
  MYSQL_STMT *stmt, *stmt1;
  int        rc;
  int        set_count, def_count, get_count;
  ulong      length;
  char       var[NAME_LEN+1];
  MYSQL_BIND set_bind[1], get_bind[2];

  myheader("test_set_variable");

  mysql_autocommit(mysql, TRUE);

  stmt1= mysql_simple_prepare(mysql, "show variables like 'max_error_count'");
  check_stmt(stmt1);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) get_bind, sizeof(get_bind));

  get_bind[0].buffer_type= MYSQL_TYPE_STRING;
  get_bind[0].buffer= (void *)var;
  get_bind[0].length= &length;
  get_bind[0].buffer_length= (int)NAME_LEN;
  length= NAME_LEN;

  get_bind[1].buffer_type= MYSQL_TYPE_LONG;
  get_bind[1].buffer= (void *)&get_count;

  rc= mysql_stmt_execute(stmt1);
  check_execute(stmt1, rc);

  rc= mysql_stmt_bind_result(stmt1, get_bind);
  check_execute(stmt1, rc);

  rc= mysql_stmt_fetch(stmt1);
  check_execute(stmt1, rc);

  if (!opt_silent)
    fprintf(stdout, "\n max_error_count(default): %d", get_count);
  def_count= get_count;

  DIE_UNLESS(strcmp(var, "max_error_count") == 0);
  rc= mysql_stmt_fetch(stmt1);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  stmt= mysql_simple_prepare(mysql, "set max_error_count= ?");
  check_stmt(stmt);

  bzero((char*) set_bind, sizeof(set_bind));

  set_bind[0].buffer_type= MYSQL_TYPE_LONG;
  set_bind[0].buffer= (void *)&set_count;

  rc= mysql_stmt_bind_param(stmt, set_bind);
  check_execute(stmt, rc);

  set_count= 31;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_commit(mysql);

  rc= mysql_stmt_execute(stmt1);
  check_execute(stmt1, rc);

  rc= mysql_stmt_fetch(stmt1);
  check_execute(stmt1, rc);

  if (!opt_silent)
    fprintf(stdout, "\n max_error_count         : %d", get_count);
  DIE_UNLESS(get_count == set_count);

  rc= mysql_stmt_fetch(stmt1);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* restore back to default */
  set_count= def_count;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt1);
  check_execute(stmt1, rc);

  rc= mysql_stmt_fetch(stmt1);
  check_execute(stmt1, rc);

  if (!opt_silent)
    fprintf(stdout, "\n max_error_count(default): %d", get_count);
  DIE_UNLESS(get_count == set_count);

  rc= mysql_stmt_fetch(stmt1);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
  mysql_stmt_close(stmt1);
}