TEST_F(
    RenameCollectionTest,
    RenameCollectionForApplyOpsReturnsNamespaceNotFoundIfTargetUuidRefersToDropPendingCollection) {
    repl::OpTime dropOpTime(Timestamp(Seconds(100), 0), 1LL);
    auto dropPendingNss = _sourceNss.makeDropPendingNamespace(dropOpTime);
    auto options = _makeCollectionOptionsWithUuid();
    _createCollection(_opCtx.get(), dropPendingNss, options);

    auto dbName = _sourceNss.db().toString();
    NamespaceString ignoredSourceNss(dbName, "ignored");
    auto uuidDoc = options.uuid->toBSON();
    auto cmd = BSON("renameCollection" << ignoredSourceNss.ns() << "to" << _targetNss.ns());
    ASSERT_EQUALS(ErrorCodes::NamespaceNotFound,
                  renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["uuid"], cmd, {}));

    // Source collections stays in drop-pending state.
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), ignoredSourceNss));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dropPendingNss));
}