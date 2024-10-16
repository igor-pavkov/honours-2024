TEST_F(RenameCollectionTest, RenameCollectionAcrossDatabaseOplogEntriesDropTarget) {
    _createCollection(_opCtx.get(), _targetNssDifferentDb);
    bool forApplyOps = false;
    _testRenameCollectionAcrossDatabaseOplogEntries(
        _opCtx.get(),
        _sourceNss,
        _targetNssDifferentDb,
        &_opObserver->oplogEntries,
        forApplyOps,
        {"create", "index", "inserts", "rename", "drop"});
}