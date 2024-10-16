static void test_bug6096()
{
  MYSQL_STMT *stmt;
  MYSQL_RES *query_result, *stmt_metadata;
  const char *stmt_text;
  MYSQL_BIND my_bind[12];
  MYSQL_FIELD *query_field_list, *stmt_field_list;
  ulong query_field_count, stmt_field_count;
  int rc;
  my_bool update_max_length= TRUE;
  uint i;

  myheader("test_bug6096");

  stmt_text= "drop table if exists t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  mysql_query(mysql, "set sql_mode=''");
  stmt_text= "create table t1 (c_tinyint tinyint, c_smallint smallint, "
                             " c_mediumint mediumint, c_int int, "
                             " c_bigint bigint, c_float float, "
                             " c_double double, c_varchar varchar(20), "
                             " c_char char(20), c_time time, c_date date, "
                             " c_datetime datetime)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "insert into t1  values (-100, -20000, 30000000, 4, 8, 1.0, "
                                     "2.0, 'abc', 'def', now(), now(), now())";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "select * from t1";

  /* Run select in prepared and non-prepared mode and compare metadata */
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  query_result= mysql_store_result(mysql);
  query_field_list= mysql_fetch_fields(query_result);
  query_field_count= mysql_num_fields(query_result);

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH,
                      (void*) &update_max_length);
  mysql_stmt_store_result(stmt);
  stmt_metadata= mysql_stmt_result_metadata(stmt);
  stmt_field_list= mysql_fetch_fields(stmt_metadata);
  stmt_field_count= mysql_num_fields(stmt_metadata);
  DIE_UNLESS(stmt_field_count == query_field_count);

  /* Print out and check the metadata */

  if (!opt_silent)
  {
    printf(" ------------------------------------------------------------\n");
    printf("             |                     Metadata \n");
    printf(" ------------------------------------------------------------\n");
    printf("             |         Query          |   Prepared statement \n");
    printf(" ------------------------------------------------------------\n");
    printf(" field name  |  length   | max_length |  length   |  max_length\n");
    printf(" ------------------------------------------------------------\n");

    for (i= 0; i < query_field_count; ++i)
    {
      MYSQL_FIELD *f1= &query_field_list[i], *f2= &stmt_field_list[i];
      printf(" %-11s | %9lu | %10lu | %9lu | %10lu \n",
             f1->name, f1->length, f1->max_length, f2->length, f2->max_length);
      DIE_UNLESS(f1->length == f2->length);
    }
    printf(" ---------------------------------------------------------------\n");
  }

  /* Bind and fetch the data */

  bzero((char*) my_bind, sizeof(my_bind));
  for (i= 0; i < stmt_field_count; ++i)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_STRING;
    my_bind[i].buffer_length= stmt_field_list[i].max_length + 1;
    my_bind[i].buffer= malloc(my_bind[i].buffer_length);
  }
  mysql_stmt_bind_result(stmt, my_bind);
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* Clean up */

  for (i= 0; i < stmt_field_count; ++i)
    free(my_bind[i].buffer);
  mysql_stmt_close(stmt);
  mysql_free_result(query_result);
  mysql_free_result(stmt_metadata);
  stmt_text= "drop table t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}