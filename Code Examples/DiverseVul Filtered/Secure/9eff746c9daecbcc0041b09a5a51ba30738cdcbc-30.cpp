static int _sqlite_rollback_txn(void *db, const sasl_utils_t *utils)
{
    return _sqlite_exec(db, "ROLLBACK TRANSACTION", NULL, 0, NULL, utils);
}