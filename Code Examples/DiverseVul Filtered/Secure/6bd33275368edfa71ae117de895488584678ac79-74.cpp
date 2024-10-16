int mboxlist_abort(struct txn *tid)
{
    assert(tid);

    return cyrusdb_abort(mbdb, tid);
}