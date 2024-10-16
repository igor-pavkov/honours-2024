TEST_F(RenameCollectionTest,
       RenameCollectionForApplyOpsReturnsNamespaceNotFoundIfSourceCollectionIsDropPending) {
    repl::OpTime dropOpTime(Timestamp(Seconds(100), 0), 1LL);
    auto dropPendingNss = _sourceNss.makeDropPendingNamespace(dropOpTime);
    _createCollection(_opCtx.get(), dropPendingNss);

    auto dbName = _sourceNss.db().toString();
    auto cmd = BSON("renameCollection" << dropPendingNss.ns() << "to" << _targetNss.ns());
    ASSERT_EQUALS(ErrorCodes::NamespaceNotFound,
                  renameCollectionForApplyOps(_opCtx.get(), dbName, {}, cmd, {}));

    // Source collections stays in drop-pending state.
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dropPendingNss));
}