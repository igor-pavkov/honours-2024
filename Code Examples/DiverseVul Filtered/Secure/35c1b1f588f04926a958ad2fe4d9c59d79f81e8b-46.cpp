TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsAcrossDatabaseOplogEntriesDropTarget) {
    _createCollection(_opCtx.get(), _targetNssDifferentDb);
    bool forApplyOps = true;
    _testRenameCollectionAcrossDatabaseOplogEntries(
        _opCtx.get(),
        _sourceNss,
        _targetNssDifferentDb,
        &_opObserver->oplogEntries,
        forApplyOps,
        {"create", "index", "inserts", "rename", "drop"});
}