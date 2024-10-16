TEST_F(RenameCollectionTest, RenameAcrossDatabasesDoesNotPreserveCatalogPointers) {
    _createCollection(_opCtx.get(), _sourceNss);
    Lock::GlobalWrite globalWrite(_opCtx.get());

    // Get a pointer to the source collection, and ensure that it reports the expected namespace
    // string.
    Collection* sourceColl = _getCollection_inlock(_opCtx.get(), _sourceNss);
    ASSERT(sourceColl);
    auto* sourceCatalogEntry = sourceColl->getCatalogEntry();
    ASSERT(sourceCatalogEntry);

    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}));

    // Verify that the CollectionCatalogEntry reports that its namespace is now the target
    // namespace.
    Collection* targetColl = _getCollection_inlock(_opCtx.get(), _targetNssDifferentDb);
    ASSERT(targetColl);
    ASSERT_NE(targetColl, sourceColl);
    auto* targetCatalogEntry = targetColl->getCatalogEntry();
    ASSERT(targetCatalogEntry);
    ASSERT_NE(targetCatalogEntry, sourceCatalogEntry);
}