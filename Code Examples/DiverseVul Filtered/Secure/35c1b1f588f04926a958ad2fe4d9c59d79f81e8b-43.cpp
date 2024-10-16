TEST_F(RenameCollectionTest, LongIndexNameAllowedForTargetCollection) {
    ASSERT_GREATER_THAN(_targetNssDifferentDb.size(), _sourceNss.size());
    std::size_t longestIndexNameAllowedForSource =
        NamespaceString::MaxNsLen - 2U /*strlen(".$")*/ - _sourceNss.size();
    ASSERT_OK(_sourceNss.checkLengthForRename(longestIndexNameAllowedForSource));
    ASSERT_EQUALS(ErrorCodes::InvalidLength,
                  _targetNssDifferentDb.checkLengthForRename(longestIndexNameAllowedForSource));

    _createCollection(_opCtx.get(), _sourceNss);
    const std::string indexName(longestIndexNameAllowedForSource, 'a');
    _createIndexOnEmptyCollection(_opCtx.get(), _sourceNss, indexName);
    ASSERT_OK(renameCollection(_opCtx.get(), _sourceNss, _targetNssDifferentDb, {}));
}