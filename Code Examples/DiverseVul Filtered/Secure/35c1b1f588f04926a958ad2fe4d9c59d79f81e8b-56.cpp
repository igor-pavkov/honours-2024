TEST_F(RenameCollectionTest,
       RenameCollectionForApplyOpsReturnsInvalidNamespaceIfTargetNamespaceIsInvalid) {
    _createCollection(_opCtx.get(), _sourceNss);
    auto dbName = _sourceNss.db().toString();

    // Create a namespace that is not in the form "database.collection".
    NamespaceString invalidTargetNss("invalidNamespace");

    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << invalidTargetNss.ns());

    ASSERT_EQUALS(ErrorCodes::InvalidNamespace,
                  renameCollectionForApplyOps(_opCtx.get(), dbName, {}, cmd, {}));
}