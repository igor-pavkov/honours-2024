static int _mysql_rollback_txn(void *conn, const sasl_utils_t *utils)
{
    return _mysql_exec(conn, "ROLLBACK", NULL, 0, NULL, utils);
}