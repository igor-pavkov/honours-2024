TEST_F(RenameCollectionTest,
       RenameCollectionOverridesDropTargetIfTargetCollectionIsMissingAndDropTargetIsTrue) {
    _createCollectionWithUUID(_opCtx.get(), _sourceNss);
    RenameCollectionOptions options;
    options.dropTarget = true;
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNss, options));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss))
        << "source collection " << _sourceNss << " still exists after successful rename";
    ASSERT_TRUE(_collectionExists(_opCtx.get(), _targetNss)) << "target collection " << _targetNss
                                                             << " missing after successful rename";

    ASSERT_TRUE(_opObserver->onRenameCollectionCalled);
    ASSERT_FALSE(_opObserver->onRenameCollectionDropTarget);
}