TEST_F(RenameCollectionTest, CollectionPointerRemainsValidThroughRename) {
    _createCollection(_opCtx.get(), _sourceNss);
    Lock::GlobalWrite globalWrite(_opCtx.get());

    // Get a pointer to the source collection, and ensure that it reports the expected namespace
    // string.
    Collection* sourceColl = _getCollection_inlock(_opCtx.get(), _sourceNss);
    ASSERT(sourceColl);

    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNss, {}));

    // Retrieve the pointer associated with the target namespace, and ensure that its the same
    // pointer (i.e. the renamed collection has the very same Collection instance).
    Collection* targetColl = _getCollection_inlock(_opCtx.get(), _targetNss);
    ASSERT(targetColl);
    ASSERT_EQ(targetColl, sourceColl);

    // Verify that the Collection reports that its namespace is now the target namespace.
    ASSERT_EQ(targetColl->ns(), _targetNss);
}