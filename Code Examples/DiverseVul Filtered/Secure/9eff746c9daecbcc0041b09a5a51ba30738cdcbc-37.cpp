static int _sqlite_commit_txn(void *db, const sasl_utils_t *utils)
{
    return _sqlite_exec(db, "COMMIT TRANSACTION", NULL, 0, NULL, utils);
}