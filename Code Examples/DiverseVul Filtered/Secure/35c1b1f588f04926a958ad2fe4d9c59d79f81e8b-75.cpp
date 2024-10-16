TEST_F(RenameCollectionTest, UUIDCatalogMappingRemainsIntactThroughRename) {
    _createCollection(_opCtx.get(), _sourceNss);
    Lock::GlobalWrite globalWrite(_opCtx.get());
    auto& uuidCatalog = UUIDCatalog::get(_opCtx.get());
    Collection* sourceColl = _getCollection_inlock(_opCtx.get(), _sourceNss);
    ASSERT(sourceColl);
    ASSERT_EQ(sourceColl, uuidCatalog.lookupCollectionByUUID(*sourceColl->uuid()));
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNss, {}));
    ASSERT_EQ(sourceColl, uuidCatalog.lookupCollectionByUUID(*sourceColl->uuid()));
}