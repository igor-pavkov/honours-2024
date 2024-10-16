TEST_F(RenameCollectionTest, TargetCollectionNameTooLong) {
    _createCollection(_opCtx.get(), _sourceNss);
    const std::string targetCollectionName(NamespaceString::MaxNsCollectionLen, 'a');
    NamespaceString longTargetNss(_sourceNss.db(), targetCollectionName);
    ASSERT_EQUALS(ErrorCodes::InvalidLength,
                  renameCollection(_opCtx.get(), _sourceNss, longTargetNss, {}));
}