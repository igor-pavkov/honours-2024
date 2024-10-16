TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsDropTargetEvenIfSourceIsDropPending) {
    repl::OpTime dropOpTime(Timestamp(Seconds(100), 0), 1LL);
    auto dropPendingNss = _sourceNss.makeDropPendingNamespace(dropOpTime);

    auto dropTargetUUID = _createCollectionWithUUID(_opCtx.get(), _targetNss);
    auto uuidDoc = BSON("ui" << _createCollectionWithUUID(_opCtx.get(), dropPendingNss));
    auto cmd =
        BSON("renameCollection" << dropPendingNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                << "true");

    repl::UnreplicatedWritesBlock uwb(_opCtx.get());
    repl::OpTime renameOpTime = {Timestamp(Seconds(200), 1U), 1LL};
    ASSERT_OK(renameCollectionForApplyOps(
        _opCtx.get(), dropPendingNss.db().toString(), uuidDoc["ui"], cmd, renameOpTime));

    // Source collections stays in drop-pending state.
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dropPendingNss));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _targetNss));
    ASSERT_EQUALS(_targetNss.makeDropPendingNamespace(renameOpTime),
                  _getCollectionNssFromUUID(_opCtx.get(), dropTargetUUID));
}