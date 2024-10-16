static void test_bug5399()
{
  /*
    Ascii 97 is 'a', which gets mapped to Ascii 65 'A' unless internal
    statement id hash in the server uses binary collation.
  */
#define NUM_OF_USED_STMT 97 
  MYSQL_STMT *stmt_list[NUM_OF_USED_STMT];
  MYSQL_STMT **stmt;
  MYSQL_BIND my_bind[1];
  char buff[600];
  int rc;
  int32 no;

  myheader("test_bug5399");

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= &no;

  for (stmt= stmt_list; stmt != stmt_list + NUM_OF_USED_STMT; ++stmt)
  {
    sprintf(buff, "select %d", (int) (stmt - stmt_list));
    *stmt= mysql_stmt_init(mysql);
    rc= mysql_stmt_prepare(*stmt, buff, strlen(buff));
    check_execute(*stmt, rc);
    mysql_stmt_bind_result(*stmt, my_bind);
  }
  if (!opt_silent)
    printf("%d statements prepared.\n", NUM_OF_USED_STMT);

  for (stmt= stmt_list; stmt != stmt_list + NUM_OF_USED_STMT; ++stmt)
  {
    rc= mysql_stmt_execute(*stmt);
    check_execute(*stmt, rc);
    rc= mysql_stmt_store_result(*stmt);
    check_execute(*stmt, rc);
    rc= mysql_stmt_fetch(*stmt);
    DIE_UNLESS(rc == 0);
    DIE_UNLESS((int32) (stmt - stmt_list) == no);
  }

  for (stmt= stmt_list; stmt != stmt_list + NUM_OF_USED_STMT; ++stmt)
    mysql_stmt_close(*stmt);
#undef NUM_OF_USED_STMT
}