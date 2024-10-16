TEST_F(RenameCollectionTest, RenameCollectionAcrossDatabaseWithUuid) {
    auto options = _makeCollectionOptionsWithUuid();
    _createCollection(_opCtx.get(), _sourceNss, options);
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), _sourceNss));
    ASSERT(options.uuid);
    ASSERT_NOT_EQUALS(*options.uuid, _getCollectionUuid(_opCtx.get(), _targetNssDifferentDb));
}