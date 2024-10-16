TEST_F(RenameCollectionTest, RenameCollectionMakesTargetCollectionDropPendingIfDropTargetIsTrue) {
    _createCollectionWithUUID(_opCtx.get(), _sourceNss);
    auto targetUUID = _createCollectionWithUUID(_opCtx.get(), _targetNss);
    RenameCollectionOptions options;
    options.dropTarget = true;
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNss, options));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss))
        << "source collection " << _sourceNss << " still exists after successful rename";
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _targetNss)) << "target collection " << _targetNss
                                                             << " missing after successful rename";

    ASSERT_TRUE(_opObserver->onRenameCollectionCalled);
    ASSERT(_opObserver->onRenameCollectionDropTarget);
    ASSERT_EQUALS(targetUUID, *_opObserver->onRenameCollectionDropTarget);

    auto renameOpTime = _opObserver->renameOpTime;
    ASSERT_GREATER_THAN(renameOpTime, repl::OpTime());

    // Confirm that the target collection has been renamed to a drop-pending collection.
    auto dpns = _targetNss.makeDropPendingNamespace(renameOpTime);
    ASSERT_TRUE(_collectionExists(_opCtx.get(), dpns))
        << "target collection " << _targetNss
        << " not renamed to drop-pending collection after successful rename";
}