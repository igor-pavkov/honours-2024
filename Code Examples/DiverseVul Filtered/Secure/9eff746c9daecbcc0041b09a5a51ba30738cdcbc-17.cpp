static int _mysql_begin_txn(void *conn, const sasl_utils_t *utils)
{
    return _mysql_exec(conn,
#if MYSQL_VERSION_ID >= 40011
		       "START TRANSACTION",
#else
		       "BEGIN",
#endif
		       NULL, 0, NULL, utils);
}