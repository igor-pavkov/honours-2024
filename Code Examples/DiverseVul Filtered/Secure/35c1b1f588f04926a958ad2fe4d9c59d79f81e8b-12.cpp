TEST_F(RenameCollectionTest, FailRenameCollectionFromUnreplicatedToReplicatedDB) {
    NamespaceString sourceNss("foo.system.profile");
    NamespaceString targetNss("foo.bar");

    _createCollection(_opCtx.get(), sourceNss);

    ASSERT_EQUALS(ErrorCodes::IllegalOperation,
                  renameCollection(_opCtx.get(), sourceNss, targetNss, {}));
}