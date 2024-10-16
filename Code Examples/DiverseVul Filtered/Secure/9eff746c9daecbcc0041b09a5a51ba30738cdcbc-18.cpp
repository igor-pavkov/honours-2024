static int _mysql_commit_txn(void *conn, const sasl_utils_t *utils)
{
    return _mysql_exec(conn, "COMMIT", NULL, 0, NULL, utils);
}