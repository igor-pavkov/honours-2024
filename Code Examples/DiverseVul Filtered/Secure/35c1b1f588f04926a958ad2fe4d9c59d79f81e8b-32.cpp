bool _isTempCollection(OperationContext* opCtx, const NamespaceString& nss) {
    AutoGetCollectionForRead autoColl(opCtx, nss);
    auto collection = autoColl.getCollection();
    ASSERT_TRUE(collection) << "Unable to check if " << nss
                            << " is a temporary collection because collection does not exist.";
    auto catalogEntry = collection->getCatalogEntry();
    auto options = catalogEntry->getCollectionOptions(opCtx);
    return options.temp;
}