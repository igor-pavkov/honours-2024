CollectionUUID _getCollectionUuid(OperationContext* opCtx, const NamespaceString& nss) {
    auto options = _getCollectionOptions(opCtx, nss);
    ASSERT_TRUE(options.uuid);
    return *(options.uuid);
}