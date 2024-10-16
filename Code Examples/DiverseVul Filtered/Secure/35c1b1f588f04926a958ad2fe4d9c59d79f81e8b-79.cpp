TEST_F(RenameCollectionTest,
       RenameCollectionAcrossDatabaseDowngradesGlobalWriteLockToNonExclusive) {
    _createCollection(_opCtx.get(), _sourceNss);
    _insertDocument(_opCtx.get(), _sourceNss, BSON("_id" << 0));
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}));
    ASSERT_FALSE(_opObserver->onInsertsIsGlobalWriteLockExclusive);
}