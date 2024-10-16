TEST_F(RenameCollectionTest,
       RenameCollectionAcrossDatabaseKeepsGlobalWriteLockExclusiveIfCallerHasGlobalWriteLock) {
    // This simulates the case when renameCollection is called using the applyOps command (different
    // from secondary oplog application).
    _createCollection(_opCtx.get(), _sourceNss);
    _insertDocument(_opCtx.get(), _sourceNss, BSON("_id" << 0));
    Lock::GlobalWrite globalWrite(_opCtx.get());
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}));
    ASSERT_TRUE(_opObserver->onInsertsIsGlobalWriteLockExclusive);
}