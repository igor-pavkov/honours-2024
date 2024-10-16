TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsAcrossDatabaseOplogEntries) {
    bool forApplyOps = true;
    _testRenameCollectionAcrossDatabaseOplogEntries(
        _opCtx.get(),
        _sourceNss,
        _targetNssDifferentDb,
        &_opObserver->oplogEntries,
        forApplyOps,
        {"create", "index", "inserts", "rename", "drop"});
}