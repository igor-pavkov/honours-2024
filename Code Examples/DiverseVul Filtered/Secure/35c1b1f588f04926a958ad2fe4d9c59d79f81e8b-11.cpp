CollectionOptions _getCollectionOptions(OperationContext* opCtx, const NamespaceString& nss) {
    AutoGetCollectionForRead autoColl(opCtx, nss);
    auto collection = autoColl.getCollection();
    ASSERT_TRUE(collection) << "Unable to get collections options for " << nss
                            << " because collection does not exist.";
    auto catalogEntry = collection->getCatalogEntry();
    return catalogEntry->getCollectionOptions(opCtx);
}