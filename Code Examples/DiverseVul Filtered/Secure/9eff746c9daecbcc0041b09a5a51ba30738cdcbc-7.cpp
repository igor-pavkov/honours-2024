static int _pgsql_rollback_txn(void *conn, const sasl_utils_t *utils)
{
    return _pgsql_exec(conn, "ROLLBACK;", NULL, 0, NULL, utils);
}