TEST_F(RenameCollectionTest,
       RenameCollectionReturnsNamespaceNotFoundIfSourceCollectionIsDropPending) {
    repl::OpTime dropOpTime(Timestamp(Seconds(100), 0), 1LL);
    auto dropPendingNss = _sourceNss.makeDropPendingNamespace(dropOpTime);

    _createCollection(_opCtx.get(), dropPendingNss);
    ASSERT_EQUALS(ErrorCodes::NamespaceNotFound,
                  renameCollection(_opCtx.get(), dropPendingNss, _targetNss, {}));

    // Source collections stays in drop-pending state.
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dropPendingNss));
}