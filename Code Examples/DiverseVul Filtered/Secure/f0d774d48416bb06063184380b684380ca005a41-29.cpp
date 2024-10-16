static void mysql_close_free(MYSQL *mysql)
{
  my_free(mysql->host_info);
  my_free(mysql->user);
  my_free(mysql->passwd);
  my_free(mysql->db);
#if defined(EMBEDDED_LIBRARY) || MYSQL_VERSION_ID >= 50100
  my_free(mysql->info_buffer);
  mysql->info_buffer= 0;
#endif
  /* Clear pointers for better safety */
  mysql->host_info= mysql->user= mysql->passwd= mysql->db= 0;
}