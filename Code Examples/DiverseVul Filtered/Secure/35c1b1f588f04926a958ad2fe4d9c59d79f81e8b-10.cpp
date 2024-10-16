TEST_F(RenameCollectionTest, RenameCollectionToItselfByNsForApplyOps) {
    auto dbName = _sourceNss.db().toString();
    auto uuid = _createCollectionWithUUID(_opCtx.get(), _sourceNss);
    auto uuidDoc = BSON("ui" << uuid);
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _sourceNss.ns() << "dropTarget"
                                       << true);
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["ui"], cmd, {}));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _sourceNss));
}