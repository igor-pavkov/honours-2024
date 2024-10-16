static int _pgsql_begin_txn(void *conn, const sasl_utils_t *utils)
{
    return _pgsql_exec(conn, "BEGIN;", NULL, 0, NULL, utils);
}