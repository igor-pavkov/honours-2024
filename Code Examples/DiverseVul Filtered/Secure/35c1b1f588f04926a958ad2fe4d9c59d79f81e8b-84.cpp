TEST_F(RenameCollectionTest,
       RenameCollectionForApplyOpsMakesTargetCollectionDropPendingIfDropTargetIsTrue) {
    repl::UnreplicatedWritesBlock uwb(_opCtx.get());
    ASSERT_FALSE(_opCtx->writesAreReplicated());

    // OpObserver::preRenameCollection() must return a null OpTime when writes are not replicated.
    _opObserver->renameOpTime = {};

    _createCollection(_opCtx.get(), _sourceNss);
    _createCollection(_opCtx.get(), _targetNss);
    auto dbName = _sourceNss.db().toString();
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns() << "dropTarget"
                                       << true);

    repl::OpTime renameOpTime = {Timestamp(Seconds(200), 1U), 1LL};
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, {}, cmd, renameOpTime));

    // Confirm that the target collection has been renamed to a drop-pending collection.
    auto dpns = _targetNss.makeDropPendingNamespace(renameOpTime);
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dpns))
        << "target collection " << _targetNss
        << " not renamed to drop-pending collection after successful rename for applyOps";
}