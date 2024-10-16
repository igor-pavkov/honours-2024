static void _pgsql_close(void *conn)
{
    PQfinish(conn);
}