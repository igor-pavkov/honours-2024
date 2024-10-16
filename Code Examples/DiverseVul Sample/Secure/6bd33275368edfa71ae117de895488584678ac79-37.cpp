EXPORTED int mboxlist_commit(struct txn *tid)
{
    assert(tid);

    return cyrusdb_commit(mbdb, tid);
}