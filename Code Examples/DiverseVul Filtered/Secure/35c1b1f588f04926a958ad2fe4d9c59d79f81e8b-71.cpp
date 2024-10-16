TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsDropTargetByUUIDEvenIfSourceDoesNotExist) {
    auto missingSourceNss = NamespaceString("test.bar2");
    auto dropTargetNss = NamespaceString("test.bar3");
    _createCollectionWithUUID(_opCtx.get(), _targetNss);
    auto dropTargetUUID = _createCollectionWithUUID(_opCtx.get(), dropTargetNss);
    auto uuidDoc = BSON("ui" << UUID::gen());
    auto cmd =
        BSON("renameCollection" << missingSourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                << dropTargetUUID);
    ASSERT_OK(renameCollectionForApplyOps(
        _opCtx.get(), missingSourceNss.db().toString(), uuidDoc["ui"], cmd, {}));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _targetNss));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), dropTargetNss));
}