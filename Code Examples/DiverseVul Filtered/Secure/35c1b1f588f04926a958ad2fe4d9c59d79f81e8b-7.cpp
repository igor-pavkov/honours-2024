void _testRenameCollectionAcrossDatabaseOplogEntries(
    OperationContext* opCtx,
    const NamespaceString& sourceNss,
    const NamespaceString& targetNss,
    std::vector<std::string>* oplogEntries,
    bool forApplyOps,
    const std::vector<std::string>& expectedOplogEntries) {
    ASSERT_NOT_EQUALS(sourceNss.db(), targetNss.db());
    _createCollection(opCtx, sourceNss);
    _createIndexOnEmptyCollection(opCtx, sourceNss, "a_1");
    _insertDocument(opCtx, sourceNss, BSON("_id" << 0));
    oplogEntries->clear();
    if (forApplyOps) {
        auto cmd = BSON(
            "renameCollection" << sourceNss.ns() << "to" << targetNss.ns() << "dropTarget" << true);
        ASSERT_OK(renameCollectionForApplyOps(opCtx, sourceNss.db().toString(), {}, cmd, {}));
    } else {
        RenameCollectionOptions options;
        options.dropTarget = true;
        ASSERT_OK(renameCollection(opCtx, sourceNss, targetNss, options));
    }
    _checkOplogEntries(*oplogEntries, expectedOplogEntries);
}