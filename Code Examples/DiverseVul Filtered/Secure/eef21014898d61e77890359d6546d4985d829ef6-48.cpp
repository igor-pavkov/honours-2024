static void test_parse_error_and_bad_length()
{
  MYSQL_STMT *stmt;
  int rc;

  /* check that we get 4 syntax errors over the 4 calls */
  myheader("test_parse_error_and_bad_length");

  rc= mysql_query(mysql, "SHOW DATABAAAA");
  DIE_UNLESS(rc);
  if (!opt_silent)
    fprintf(stdout, "Got error (as expected): '%s'\n", mysql_error(mysql));
  rc= mysql_real_query(mysql, STRING_WITH_LEN("SHOW DATABASES\0AAAAAAAA"));
  DIE_UNLESS(rc);
  if (!opt_silent)
    fprintf(stdout, "Got error (as expected): '%s'\n", mysql_error(mysql));

  stmt= mysql_simple_prepare(mysql, "SHOW DATABAAAA");
  DIE_UNLESS(!stmt);
  if (!opt_silent)
    fprintf(stdout, "Got error (as expected): '%s'\n", mysql_error(mysql));
  stmt= mysql_stmt_init(mysql);
  DIE_UNLESS(stmt);
  rc= mysql_stmt_prepare(stmt, STRING_WITH_LEN("SHOW DATABASES\0AAAAAAA"));
  DIE_UNLESS(rc != 0);
  if (!opt_silent)
    fprintf(stdout, "Got error (as expected): '%s'\n", mysql_stmt_error(stmt));
  mysql_stmt_close(stmt);
}