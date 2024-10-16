TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsSourceAndTargetDoNotExist) {
    auto uuidDoc = BSON("ui" << UUID::gen());
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                       << "true");
    ASSERT_EQUALS(ErrorCodes::NamespaceNotFound,
                  renameCollectionForApplyOps(
                      _opCtx.get(), _sourceNss.db().toString(), uuidDoc["ui"], cmd, {}));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
}