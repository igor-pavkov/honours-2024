TEST_F(RenameCollectionTest, RenameCollectionAcrossDatabaseDropsTemporaryCollectionOnException) {
    _createCollection(_opCtx.get(), _sourceNss);
    _createIndexOnEmptyCollection(_opCtx.get(), _sourceNss, "a_1");
    _insertDocument(_opCtx.get(), _sourceNss, BSON("_id" << 0));
    _opObserver->onInsertsThrows = true;
    _opObserver->oplogEntries.clear();
    ASSERT_THROWS_CODE(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}),
                       AssertionException,
                       ErrorCodes::OperationFailed);
    _checkOplogEntries(_opObserver->oplogEntries, {"create", "index", "drop"});
}