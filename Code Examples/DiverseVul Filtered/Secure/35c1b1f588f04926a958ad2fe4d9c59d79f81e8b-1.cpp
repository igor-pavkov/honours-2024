TEST_F(RenameCollectionTest, RenameCollectionAcrossDatabaseOplogEntries) {
    bool forApplyOps = false;
    _testRenameCollectionAcrossDatabaseOplogEntries(
        _opCtx.get(),
        _sourceNss,
        _targetNssDifferentDb,
        &_opObserver->oplogEntries,
        forApplyOps,
        {"create", "index", "inserts", "rename", "drop"});
}