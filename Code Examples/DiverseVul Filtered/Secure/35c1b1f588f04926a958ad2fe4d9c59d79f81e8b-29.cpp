TEST_F(RenameCollectionTest, RenameCollectionByUUIDRatherThanNsForApplyOps) {
    auto realRenameFromNss = NamespaceString("test.bar2");
    auto dbName = realRenameFromNss.db().toString();
    auto uuid = _createCollectionWithUUID(_opCtx.get(), realRenameFromNss);
    auto uuidDoc = BSON("ui" << uuid);
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                       << true);
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["ui"], cmd, {}));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _targetNss));
}