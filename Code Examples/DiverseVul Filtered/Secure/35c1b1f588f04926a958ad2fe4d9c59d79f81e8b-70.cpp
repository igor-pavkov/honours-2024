TEST_F(RenameCollectionTest, RenameCollectionReturnsNamespaceNotFoundIfDatabaseDoesNotExist) {
    ASSERT_FALSE(AutoGetDb(_opCtx.get(), _sourceNss.db(), MODE_X).getDb());
    ASSERT_EQUALS(ErrorCodes::NamespaceNotFound,
                  renameCollection(_opCtx.get(), _sourceNss, _targetNss, {}));
}