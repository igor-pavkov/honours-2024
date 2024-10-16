TEST_F(RenameCollectionTest, RenameCollectionToItselfByUUIDForApplyOps) {
    auto dbName = _targetNss.db().toString();
    auto uuid = _createCollectionWithUUID(_opCtx.get(), _targetNss);
    auto uuidDoc = BSON("ui" << uuid);
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                       << true);
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["ui"], cmd, {}));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _targetNss));
}