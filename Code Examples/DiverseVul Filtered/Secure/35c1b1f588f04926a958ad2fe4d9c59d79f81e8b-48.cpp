TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsAcrossDatabaseWithTargetUuid) {
    _createCollection(_opCtx.get(), _sourceNss);
    auto dbName = _sourceNss.db().toString();
    auto uuid = UUID::gen();
    auto uuidDoc = BSON("ui" << uuid);
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNssDifferentDb.ns()
                                       << "dropTarget"
                                       << true);
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["ui"], cmd, {}));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss));
    ASSERT_EQUALS(uuid, _getCollectionUuid(_opCtx.get(), _targetNssDifferentDb));
}