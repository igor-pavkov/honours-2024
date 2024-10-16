static my_bool is_query_cache_available()
{
  int rc;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int res= -1;

  rc= mysql_query(mysql, "SHOW VARIABLES LIKE 'have_query_cache'");
  myquery(rc);

  result= mysql_store_result(mysql);
  DIE_UNLESS(result);

  row= mysql_fetch_row(result);
  DIE_UNLESS(row != NULL);
  if (strcmp(row[1], "YES") == 0)
    res= 1;
  else if (strcmp(row[1], "NO") == 0)
    res= 0;
  mysql_free_result(result);

  DIE_UNLESS(res == 0 || res == 1);
  return res;
}