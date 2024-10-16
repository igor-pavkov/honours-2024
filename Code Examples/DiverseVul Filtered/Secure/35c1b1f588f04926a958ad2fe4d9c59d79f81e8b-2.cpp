TEST_F(RenameCollectionTest, CatalogPointersRenameValidThroughRenameForApplyOps) {
    _createCollection(_opCtx.get(), _sourceNss);
    Collection* sourceColl = AutoGetCollectionForRead(_opCtx.get(), _sourceNss).getCollection();
    ASSERT(sourceColl);

    auto uuidDoc = BSON("ui" << UUID::gen());
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns());
    ASSERT_OK(renameCollectionForApplyOps(
        _opCtx.get(), _sourceNss.db().toString(), uuidDoc["ui"], cmd, {}));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss));

    Collection* targetColl = AutoGetCollectionForRead(_opCtx.get(), _targetNss).getCollection();
    ASSERT(targetColl);
    ASSERT_EQ(targetColl, sourceColl);
    ASSERT_EQ(targetColl->ns(), _targetNss);
}