TEST_F(RenameCollectionTest, CollectionCatalogEntryPointerRemainsValidThroughRename) {
    _createCollection(_opCtx.get(), _sourceNss);
    Lock::GlobalWrite globalWrite(_opCtx.get());

    // Get a pointer to the source collection, and ensure that it reports the expected namespace
    // string.
    Collection* sourceColl = _getCollection_inlock(_opCtx.get(), _sourceNss);
    ASSERT(sourceColl);
    auto* sourceCatalogEntry = sourceColl->getCatalogEntry();
    ASSERT(sourceCatalogEntry);
    ASSERT_EQ(sourceCatalogEntry->ns(), _sourceNss);

    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNss, {}));

    // Verify that the CollectionCatalogEntry reports that its namespace is now the target
    // namespace.
    ASSERT_EQ(sourceCatalogEntry->ns(), _targetNss);
}