static void test_mdev3885() 
{
  int rc;
  MYSQL *conn;

  myheader("test_mdev3885");
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);
  rc= mysql_kill(conn, mysql_thread_id(conn));
  DIE_UNLESS(rc);
  mysql_close(conn);
}