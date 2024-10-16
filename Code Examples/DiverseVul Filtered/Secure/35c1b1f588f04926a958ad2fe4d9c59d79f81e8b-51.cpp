TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsDropTargetEvenIfSourceDoesNotExist) {
    _createCollectionWithUUID(_opCtx.get(), _targetNss);
    auto missingSourceNss = NamespaceString("test.bar2");
    auto uuidDoc = BSON("ui" << UUID::gen());
    auto cmd =
        BSON("renameCollection" << missingSourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                << "true");
    ASSERT_OK(renameCollectionForApplyOps(
        _opCtx.get(), missingSourceNss.db().toString(), uuidDoc["ui"], cmd, {}));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
}