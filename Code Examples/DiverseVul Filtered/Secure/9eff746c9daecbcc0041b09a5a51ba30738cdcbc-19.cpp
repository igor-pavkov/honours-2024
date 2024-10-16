static int _pgsql_commit_txn(void *conn, const sasl_utils_t *utils)
{
    return _pgsql_exec(conn, "COMMIT;", NULL, 0, NULL, utils);
}