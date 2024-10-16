static int _sqlite_begin_txn(void *db, const sasl_utils_t *utils)
{
    return _sqlite_exec(db, "BEGIN TRANSACTION", NULL, 0, NULL, utils);
}