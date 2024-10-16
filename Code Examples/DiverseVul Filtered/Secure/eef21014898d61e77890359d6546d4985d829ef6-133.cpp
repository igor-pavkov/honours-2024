static void test_frm_bug()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  MYSQL_RES  *result;
  MYSQL_ROW  row;
  FILE       *test_file;
  char       data_dir[FN_REFLEN];
  char       test_frm[FN_REFLEN];
  int        rc;

  myheader("test_frm_bug");

  mysql_autocommit(mysql, TRUE);

  rc= mysql_query(mysql, "drop table if exists test_frm_bug");
  myquery(rc);

  rc= mysql_query(mysql, "flush tables");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "show variables like 'datadir'");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= data_dir;
  my_bind[0].buffer_length= FN_REFLEN;
  my_bind[1]= my_bind[0];

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n data directory: %s", data_dir);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  strxmov(test_frm, data_dir, "/", current_db, "/", "test_frm_bug.frm", NullS);

  if (!opt_silent)
    fprintf(stdout, "\n test_frm: %s", test_frm);

  if (!(test_file= my_fopen(test_frm, (int) (O_RDWR | O_CREAT), MYF(MY_WME))))
  {
    fprintf(stdout, "\n ERROR: my_fopen failed for '%s'", test_frm);
    fprintf(stdout, "\n test cancelled");
    exit(1);
  }
  if (!opt_silent)
    fprintf(test_file, "this is a junk file for test");

  rc= mysql_query(mysql, "SHOW TABLE STATUS like 'test_frm_bug'");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);/* It can't be NULL */

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);

  mysql_data_seek(result, 0);

  row= mysql_fetch_row(result);
  mytest(row);

  if (!opt_silent)
    fprintf(stdout, "\n Comment: %s", row[17]);
  DIE_UNLESS(row[17] != 0);

  mysql_free_result(result);
  mysql_stmt_close(stmt);

  my_fclose(test_file, MYF(0));
  mysql_query(mysql, "drop table if exists test_frm_bug");
}