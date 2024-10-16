TEST_F(RenameCollectionTest, FailRenameCollectionFromReplicatedToUnreplicatedDB) {
    NamespaceString sourceNss("foo.isReplicated");
    NamespaceString targetNss("local.isUnreplicated");

    _createCollection(_opCtx.get(), sourceNss);

    ASSERT_EQUALS(ErrorCodes::IllegalOperation,
                  renameCollection(_opCtx.get(), sourceNss, targetNss, {}));
}