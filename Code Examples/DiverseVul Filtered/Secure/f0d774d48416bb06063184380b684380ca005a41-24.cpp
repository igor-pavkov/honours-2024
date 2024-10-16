void STDCALL mysql_close(MYSQL *mysql)
{
  DBUG_ENTER("mysql_close");
  DBUG_PRINT("enter", ("mysql: 0x%lx", (long) mysql));

  if (mysql)					/* Some simple safety */
  {
    mysql_close_slow_part(mysql);
    mysql_close_free_options(mysql);
    mysql_close_free(mysql);
    mysql_detach_stmt_list(&mysql->stmts, "mysql_close");
#ifndef MYSQL_SERVER
    if (mysql->thd)
    {
      (*mysql->methods->free_embedded_thd)(mysql);
      mysql->thd= 0;
    }
#endif
    if (mysql->free_me)
      my_free(mysql);
  }
  DBUG_VOID_RETURN;
}